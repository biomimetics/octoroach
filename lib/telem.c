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
#include "stopwatch.h"
#include "pid.h"
#include "orient.h"
#include "dfilter_avg.h"
#include "adc_pid.h"
#include "leg_ctrl.h"
#include "sys_service.h"
#include <string.h> //for memcpy
#include "cmd.h"

#define TIMER_FREQUENCY     300                 // 400 Hz
#define TIMER_PERIOD        1/TIMER_FREQUENCY
#define DEFAULT_SKIP_NUM    2 //Default to 150 Hz save rate

#if defined(__RADIO_HIGH_DATA_RATE)
#define READBACK_DELAY_TIME_MS 3
#else
#define READBACK_DELAY_TIME_MS 10
#endif


//TODO: Remove externs by adding getters to other modules
extern pidObj motor_pidObjs[NUM_MOTOR_PIDS];
extern int bemf[NUM_MOTOR_PIDS];
extern pidObj steeringPID;

//global flag from radio module to know if last packet was ACK'd
//TODO: fix this, add a getter for the flag to radio code
extern volatile char g_last_ackd;

//Filter stuctures for gyro variables
extern filterAvgInt_t gyroZavg;

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

////   Public functions
////////////////////////

void telemSetup() {
    int retval;
    retval = sysServiceInstallT5(telemServiceRoutine);
    SetupTimer5();
}

void telemSetSamplesToSave(unsigned long n) {
    samplesToSave = n;
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
            delaytime_ms += 2;
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
    int gyroAvg[3];
    int gyroData[3];
    int gyroOffsets[3];
    int xldata[3];

    //float orZ[3];
    //orientGetOrZ(orZ);

    //skipcounter decrements to 0, triggering a telemetry save, and resets
    // value of skicounter
    if (skipcounter == 0) {
        if (samplesToSave > 0) {

            /////// Get Gyro data and calc average via filter
            gyroGetXYZ((unsigned char*) gyroData);
            gyroGetOffsets(gyroOffsets);
            //filterAvgUpdate(&gyroZavg,gyroData[2] - gyroOffsets[2]);
            //Only average for Z
            gyroAvg[2] = filterAvgCalc(&gyroZavg);

            /////// Get XL data
            xlGetXYZ((unsigned char*) xldata);

            //Stopwatch was already started in the cmdSpecialTelemetry function
            data.telemStruct.sampleIndex = sampIdx;
            data.telemStruct.timeStamp = (long) swatchTic();
            data.telemStruct.inputL = motor_pidObjs[0].input;
            data.telemStruct.inputR = motor_pidObjs[1].input;
            data.telemStruct.dcL = PDC1;
            data.telemStruct.dcR = PDC2;
            data.telemStruct.gyroX = gyroData[0] - gyroOffsets[0];
            data.telemStruct.gyroY = gyroData[1] - gyroOffsets[1];
            data.telemStruct.gyroZ = gyroData[2] - gyroOffsets[2];
            data.telemStruct.gyroAvg = gyroAvg[2];
            data.telemStruct.accelX = xldata[0];
            data.telemStruct.accelY = xldata[1];
            data.telemStruct.accelZ = xldata[2];
            data.telemStruct.bemfL = bemf[0];
            data.telemStruct.bemfR = bemf[1];
            data.telemStruct.sOut = steeringPID.output;
            data.telemStruct.Vbatt = adcGetVBatt();
            data.telemStruct.steerAngle = steeringPID.input;
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
                /////// Get Gyro data and calc average via filter
                gyroGetXYZ((unsigned char*) gyroData);
                gyroGetOffsets(gyroOffsets);
                //filterAvgUpdate(&gyroZavg,gyroData[2] - gyroOffsets[2]);
                //Only average for Z
                gyroAvg[2] = filterAvgCalc(&gyroZavg);

                /////// Get XL data
                xlGetXYZ((unsigned char*) xldata);

                //Stopwatch was already started in the cmdSpecialTelemetry function
                data.telemStruct.sampleIndex = sampIdx;
                data.telemStruct.timeStamp = (long) swatchTic();
                data.telemStruct.inputL = motor_pidObjs[0].input;
                data.telemStruct.inputR = motor_pidObjs[1].input;
                data.telemStruct.dcL = PDC1;
                data.telemStruct.dcR = PDC2;
                data.telemStruct.gyroX = gyroData[0] - gyroOffsets[0];
                data.telemStruct.gyroY = gyroData[1] - gyroOffsets[1];
                data.telemStruct.gyroZ = gyroData[2] - gyroOffsets[2];
                data.telemStruct.gyroAvg = gyroAvg[2];
                data.telemStruct.accelX = xldata[0];
                data.telemStruct.accelY = xldata[1];
                data.telemStruct.accelZ = xldata[2];
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

void telemStartStreaming(unsigned long num) {
    telemStreamingFlag = TELEM_STREAM_ON;
    samplesToStream = num;
}