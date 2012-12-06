#include "utils.h"
#include "dfmem.h"
#include "telem.h"
#include "payload.h"
#include "radio.h"
#include "at86rf231.h"
#include "ipspi1.h"
#include "led.h"
#include "gyro.h"
#include "xl.h"
#include "sclock.h"
#include "pid.h"
#include "orient.h"
#include "dfilter_avg.h"
#include "adc_pid.h"
#include "leg_ctrl.h"
#include "sys_service.h"
#include "ams-enc.h"
#include "imu.h"
#include "cmd.h" //for CMD codes
#include <string.h> //for memcpy

#define TIMER_FREQUENCY     300                 // 400 Hz
#define TIMER_PERIOD        1/TIMER_FREQUENCY
#define DEFAULT_SKIP_NUM    2 //Default to 150 Hz save rate
//#define LSB2DEG    0.0695652174 //DEFINED IN TWO PLACES!

#if defined(__RADIO_HIGH_DATA_RATE)
#define READBACK_DELAY_TIME_MS 3
#else
#define READBACK_DELAY_TIME_MS 8
#endif


// TODO (apullin) : Remove externs by adding getters to other modules
extern pidObj motor_pidObjs[NUM_MOTOR_PIDS];
extern int bemf[NUM_MOTOR_PIDS];
extern pidObj steeringPID;
extern pidObj tailPID;

//global flag from radio module to know if last packet was ACK'd
// TODO (apullin) : fix this, add a getter for the flag to radio code
extern volatile char g_last_ackd;


extern float lastTailPos;
extern float tailTorque;

extern long motor_count[2];

float telemGyroValue = 0.0;

////////   Private variables   ////////////////
static unsigned long samplesToSave = 0;
//Skip counter for dividing the 300hz timer into lower telemetry rates
static unsigned int telemSkipNum = DEFAULT_SKIP_NUM;
static unsigned int skipcounter = DEFAULT_SKIP_NUM;
static unsigned long sampIdx = 0;

static unsigned long samplesToStream = 0;
static char telemStreamingFlag = TELEM_STREAM_OFF;
static unsigned int streamSkipCounter = 0;
static unsigned int streamSkipNum = 15;

//Offset for time value when recording samples
static unsigned long telemStartTime = 0;

//Function to be installed into T5, and setup function
static void SetupTimer5(); // Might collide with setup in steering module!
static void telemServiceRoutine(void); //To be installed with sysService
//The following local functions are called by the service routine:
static void telemISRHandler(void);

/////////        Telemtry ISR          ////////
////////  Installed to Timer5 @ 300hz  ////////
//void __attribute__((interrupt, no_auto_psv)) _T5Interrupt(void) {

static void telemServiceRoutine(void) {
    //This intermediate function is used in case we want to tie other
    //sub-taks to the telemtry service routine.
    //TODO: Is this neccesary?

    // Section for saving telemetry data to flash
    // Uses telemSkip as a divisor to T5.
    telemISRHandler();
}

static void SetupTimer5() {
    ///// Timer 5 setup, Steering ISR, 300Hz /////
    // period value = Fcy/(prescale*Ftimer)
    unsigned int T5CON1value, T5PERvalue;
    // prescale 1:64
    T5CON1value = T5_ON & T5_IDLE_CON & T5_GATE_OFF & T5_PS_1_64 & T5_SOURCE_INT;
    // Period is set so that period = 5ms (200Hz), MIPS = 40
    //period = 3125; // 200Hz
    T5PERvalue = 2083; // ~300Hz
    int retval;
    retval = sysServiceConfigT5(T5CON1value, T5PERvalue, T5_INT_PRIOR_5 & T5_INT_ON);
    //OpenTimer5(con_reg, period);
    //ConfigIntTimer5(T5_INT_PRIOR_5 & T5_INT_ON);
}

void telemSetup() {
    int retval;
    retval = sysServiceInstallT5(telemServiceRoutine);
    SetupTimer5();
}

void telemSetSamplesToSave(unsigned long n) {
    samplesToSave = n;
    sampIdx = 0;
}

void telemReadbackSamples(unsigned long numSamples) {
    //unsigned int page, bufferByte;// maxpage;
    //unsigned char dataPacket[PACKETSIZE];

    int delaytime_ms = READBACK_DELAY_TIME_MS;

    unsigned long i = 0; //will actually be the same as the sampleIndex

    LED_GREEN = 1;
    //Disable motion interrupts for readback
    //_T1IE = 0; _T5IE=0; //TODO: what is a cleaner way to do this?


    telemStruct_t sampleData;

    for (i = 0; i < numSamples; i++) {
        //Retireve data from flash
        //dfmemReadSample(i, sizeof(sampleData), (unsigned char*)(&sampleData));
        dfmemReadSample(i, sizeof (sampleData), (unsigned char*) (&sampleData));

        //Reliable send, with linear backoff
        g_last_ackd = 0;
        do {
            telemSendDataDelay(PACKETSIZE, (unsigned char*) (&sampleData), delaytime_ms);
            //trx_status = phyReadBit(SR_TRAC_STATUS);
            //Linear backoff
            delaytime_ms += 1;
        } while (g_last_ackd == 0);
        delaytime_ms = READBACK_DELAY_TIME_MS;
    }

    LED_GREEN = 0;
}

void telemSendDataDelay(unsigned char data_length, unsigned char* data, int delaytime_ms) {
    // Create Payload, set status and type (don't cares)
    Payload pld = payCreateEmpty(data_length);
    //////    FIX THIS //////////
    paySetType(pld, CMD_SPECIAL_TELEMETRY); //this is the only dependance on cmd.h
    paySetStatus(pld, 0); //

    // Set Payload data
    paySetData(pld, data_length, data);

    // Send Payload WITH 15ms DELAY
    // Handles pld delete: Assigns pointer to payload in packet
    //    and radio command deletes payload, then packet.
    radioSendPayload(macGetDestAddr(), pld);
    delay_ms(delaytime_ms); // allow radio transmission time
}


//Saves telemetry data structure into flash memory, in order

void telemSaveData(telemU *data) {

    dfmemSave((unsigned char*) data, sizeof (telemU));
    samplesToSave--;

    //This is done here instead of the ISR because telemSaveData() will only be
    //executed if samplesToSave > 0 upon entry.
    if (samplesToSave == 0) {
        //Done sampling, commit last buffer
        dfmemSync();
    }
}

void telemErase(unsigned long numSamples) {
    dfmemEraseSectorsForSamples(numSamples, sizeof (telemU));
}


////   Private functions
////////////////////////

static void telemISRHandler() {
    telemU data;

    //skipcounter decrements to 0, triggering a telemetry save, and resets
    // value of skicounter
    if (skipcounter == 0) {
        if (samplesToSave > 0) {
            /////// Get XL data

            data.telemStruct.sampleIndex = sampIdx;
            data.telemStruct.timeStamp = sclockGetTime() - telemStartTime;
            data.telemStruct.inputL = motor_pidObjs[0].input;
            data.telemStruct.inputR = motor_pidObjs[1].input;
            //data.telemStruct.dcL = PDC3; //For IP2.4 modified to use Hbridge
            //data.telemStruct.dcR = PDC4; //For IP2.4 modified to use Hbridge
            data.telemStruct.dcL = PDC1;
            data.telemStruct.dcR = PDC2;
            data.telemStruct.gyroX = imuGetGyroXValue();
            data.telemStruct.gyroY = imuGetGyroYValue();
            data.telemStruct.gyroZ = imuGetGyroZValue();
            data.telemStruct.gyroAvg = imuGetGyroZValueAvgDeg();

            //XL temprorarily disabled to prevent collision with AM encoder
            // TODO (apullin, fgb, nkohut) : bring XL access into imu module
            /*data.telemStruct.accelX = xldata[0];
            data.telemStruct.accelY = xldata[1];
            data.telemStruct.accelZ = xldata[2]; */

            data.telemStruct.accelX = 0;
            data.telemStruct.accelY = 0;
            data.telemStruct.accelZ = 0;


            data.telemStruct.bemfL = bemf[0];
            data.telemStruct.bemfR = bemf[1];
            data.telemStruct.tailTorque = tailTorque;
            data.telemStruct.Vbatt = adcGetVBatt();
            data.telemStruct.steerAngle = tailPID.input;
            data.telemStruct.tailAngle = lastTailPos;
            data.telemStruct.bodyPosition = imuGetBodyZPositionDeg();
            data.telemStruct.motor_count[0] = motor_count[0];
            data.telemStruct.motor_count[1] = motor_count[1];
            data.telemStruct.sOut = steeringPID.output;
            telemSaveData(&data);
            sampIdx++;
        }
        //Reset value of skip counter
        skipcounter = telemSkipNum;
    }
    //Always decrement skip counter at every interrupt, at 300Hz
    //This way, if telemSkipNum = 1, a sample is saved at every interrupt.
    skipcounter--;



    ////////////////////   STREAMING SECTION
    if (telemStreamingFlag == TELEM_STREAM_ON) {
        if (streamSkipCounter == 0) {
            if (samplesToStream > 0) {

                /////// Get XL data
                //xlGetXYZ((unsigned char*) xldata);

                data.telemStruct.sampleIndex = sampIdx;
                data.telemStruct.timeStamp = sclockGetTime() - telemStartTime;
                data.telemStruct.inputL = motor_pidObjs[0].input;
                data.telemStruct.inputR = motor_pidObjs[1].input;
                data.telemStruct.dcL = PDC1;
                data.telemStruct.dcR = PDC2;
                data.telemStruct.gyroX = imuGetGyroXValue();
                data.telemStruct.gyroY = imuGetGyroYValue();
                data.telemStruct.gyroZ = imuGetGyroZValue();
                data.telemStruct.gyroAvg = imuGetGyroZValueAvgDeg();
                //XL temprorarily disabled to prevent collision with AM encoder
                // TODO (apullin, fgb, nkohut) : bring XL access into imu module
                data.telemStruct.accelX = 0; //xldata[0];
                data.telemStruct.accelY = 0; //xldata[1];
                data.telemStruct.accelZ = 0; //xldata[2];
                data.telemStruct.bemfL = bemf[0];
                data.telemStruct.bemfR = bemf[1];
                data.telemStruct.sOut = steeringPID.output;
                data.telemStruct.Vbatt = adcGetVBatt();
                data.telemStruct.steerAngle = steeringPID.input;
                sampIdx++;
                //Send back data:
                Payload pld;
                pld = payCreateEmpty(PACKETSIZE);
                paySetType(pld, CMD_STREAM_TELEMETRY); //requires cmd.h
                paySetStatus(pld, 0);
                //Is there a cleaner way to do this? The payload interface is confusing
                memcpy(pld->pld_data + PAYLOAD_HEADER_LENGTH, &data, sizeof (data));
                g_last_ackd = 0;
                radioSendPayload(macGetDestAddr(), pld);
                samplesToStream--;
            } else {
                telemStreamingFlag = TELEM_STREAM_OFF;
            }
            //Reset value of skip counter
            streamSkipCounter = streamSkipNum;
        }
        streamSkipCounter--; //decrement always, at 300hz
    }
}

void telemSetSkip(unsigned int skipnum) {
    telemSkipNum = skipnum;
}

//This function is a setter for the telemStartTime variable,
//which is used to offset the recorded times for telemetry, such that
//they start at approx. 0, instead of reflecting the total number of
//sclock ticks.

void telemSetStartTime(void) {
    telemStartTime = sclockGetTime();
}
