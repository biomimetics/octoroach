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

#define TIMER_FREQUENCY     200                 // 400 Hz
#define TIMER_PERIOD        1/TIMER_FREQUENCY
#define SKIP_COUNT          2

#if defined(__RADIO_HIGH_DATA_RATE)
	#define READBACK_DELAY_TIME_MS 3
#else
	#define READBACK_DELAY_TIME_MS 10
#endif


//This should get a getter function, and not use an extern
extern pidObj motor_pidObjs[NUM_MOTOR_PIDS];
extern int bemf[NUM_MOTOR_PIDS];
extern pidObj steeringPID;

//global flag from radio module to know if last packet was ACK'd
extern volatile char g_last_ackd;

//Filter stuctures for gyro variables
extern filterAvgInt_t gyroZavg;

//Private variables
static unsigned long samplesToSave = 0;
static int telemSkip = 0;

//Function to be installed into T5, and setup function
static void SetupTimer5(); // Might collide with setup in steering module!
static void telemServiceRoutine(void);  //To be installed with sysService
//The following local functions are called by the service routine:
static void telemISRHandler(void);

/////////        Telemtry ISR          ////////
////////  Installed to Timer5 @ 300hz  ////////
//void __attribute__((interrupt, no_auto_psv)) _T5Interrupt(void) {
static void telemServiceRoutine(void){
    //This intermediate function is used in case we want to tie other
    //sub-taks to the telemtry service routine.
    //TODO: Is this neccesary?

    // Section for saving telemetry data to flash
    // Uses telemSkip as a divisor to T5.
    telemISRHandler();
}

static void SetupTimer5(){
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
void telemSetup(){
    int retval;
    retval = sysServiceInstallT5(telemServiceRoutine);
    SetupTimer5();
}

void telemSetSamplesToSave(unsigned long n){
	samplesToSave = n;
}

void telemReadbackSamples(unsigned long numSamples)
{
	//unsigned int page, bufferByte;// maxpage;
	unsigned char dataPacket[PACKETSIZE + PKT_INDEX_SIZE];
	//unsigned long bytesleft = PACKETSIZE * num;
	//unsigned long telem_index = 0;
	int delaytime_ms = READBACK_DELAY_TIME_MS;

	unsigned long i;
	
	LED_GREEN = 1;
	//Disable motion interrupts for readback
	//_T1IE = 0; _T5IE=0; //TODO: what is a cleaner way to do this?
	//while(!dfmemIsReady());

	telemStruct_t sampleData;

	for(i = 0; i < numSamples; i++){
		//Retireve data from flash
		//dfmemReadSample(i, sizeof(sampleData), (unsigned char*)(&sampleData));
		dfmemReadSample(i, sizeof(sampleData), dataPacket + PKT_INDEX_SIZE);
		//Write sample number to start of packet. TODO: fix this
		*(unsigned long*)(dataPacket) = (long)i;
		//Reliable send, with linear backoff
		g_last_ackd = 0;
		do{
			telemSendDataDelay(PACKETSIZE + PKT_INDEX_SIZE, dataPacket, delaytime_ms);
			//trx_status = phyReadBit(SR_TRAC_STATUS);
			delaytime_ms += 2;
		} while(g_last_ackd == 0);
		delaytime_ms = READBACK_DELAY_TIME_MS;
		//telem_index++;
	}

	//_T1IE = 1; _T5IE=1;
	_LATB13 = 0;
}


void telemSendDataDelay(unsigned char data_length, unsigned char* data, int delaytime_ms)
{
	// Create Payload, set status and type (don't cares)
	Payload pld = payCreateEmpty(data_length);
	//////    FIX THIS //////////
    paySetType(pld, 0x89);			// Don't Care
    paySetStatus(pld, 0);		// Don't Care
    
	// Set Payload data
	paySetData(pld, data_length, data);
    
	// Send Payload WITH 15ms DELAY
	// Handles pld delete: Assigns pointer to payload in packet
	//    and radio command deletes payload, then packet.
	radioSendPayload(macGetDestAddr(), pld);
	delay_ms(delaytime_ms); 	// allow radio transmission time
}


//Saves telemetry data structure into flash memory, in order
void telemSaveData(telemU *data){
	
	dfmemSave((unsigned char*)data, sizeof(telemU));
	samplesToSave--;

	//This is done here instead of the ISR because telemSaveData() will only be
	//executed if samplesToSave > 0 upon entry.
	if(samplesToSave == 0){
		//Done sampling, commit last buffer
		dfmemSync();
	}
}


void telemErase(unsigned long numSamples){
	dfmemEraseSectorsForSamples(numSamples, sizeof(telemU));
}


////   Private functions
////////////////////////

static void telemISRHandler(){
	int samplesaved = 0;
	telemU data;
	int gyroAvg[3]; int gyroData[3]; int gyroOffsets[3];
	int xldata[3];

	//float orZ[3];
	//orientGetOrZ(orZ);
	
	if( telemSkip == 0){
		if( samplesToSave > 0)
		{
			/////// Get Gyro data and calc average via filter
			gyroGetXYZ((unsigned char*)gyroData);
			gyroGetOffsets(gyroOffsets);
			filterAvgUpdate(&gyroZavg,gyroData[2] - gyroOffsets[2]);
			//Only average for Z
			gyroAvg[2] = filterAvgCalc(&gyroZavg);
	
			/////// Get XL data
			xlGetXYZ((unsigned char*)xldata);

			//Stopwatch was already started in the cmdSpecialTelemetry function
			data.telemStruct.timeStamp = (long)swatchTic(); 
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
			samplesaved = 1;
		}
	}
	telemSkip = (telemSkip + 1) & ~SKIP_COUNT;
	//return samplesaved;
}

