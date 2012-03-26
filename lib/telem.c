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

#define TIMER_FREQUENCY     200                 // 400 Hz
#define TIMER_PERIOD        1/TIMER_FREQUENCY
#define SKIP_COUNT          2

#if defined(__RADIO_HIGH_DATA_RATE)
	#define READBACK_DELAY_TIME_MS 3
#else
	#define READBACK_DELAY_TIME_MS 10
#endif


//This should get a getter function, and not use an extern
extern pidT pidObjs[NUM_PIDS];
extern int bemf[NUM_PIDS];
extern pidT steeringPID;

//global flag from radio module to know if last packet was ACK'd
extern volatile char g_last_ackd;

//Private variables
static unsigned long samplesToSave;
static int telemSkip;

////   Private functions
////////////////////////
void telemSetSavesToSave(unsigned long n){
	samplesToSave = n;
}

////   Public functions
////////////////////////
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
	_T1IE = 0; _T5IE=0;
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

	_T1IE = 1; _T5IE=1;
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

int telemISRHandler(){
	int samplesaved = 0;
	telemU data;
	int gyroAvg[3];
	int gyroData[3];
	int gyroOffsets[3];
	int xldata[3];

	//float orZ[3];
	//orientGetOrZ(orZ);
	
	if( telemSkip == 0){
		if( samplesToSave > 0)
		{
			//Get last average value, without recalculating it
			gyroGetAvg(gyroAvg);
			gyroGetOffsets(gyroOffsets);
			gyroGetXYZ((unsigned char*)gyroData);
			//Get last XL value
			xlGetXYZ((unsigned char*)xldata);

			//Stopwatch was already started in the cmdSpecialTelemetry function
			data.telemStruct.timeStamp = (long)swatchTic(); 
			data.telemStruct.inputL = pidObjs[0].input;  
			data.telemStruct.inputR = pidObjs[1].input;
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
			data.telemStruct.Vbatt = pidObjs[0].inputOffset; //equal to adc_battery, see pid.c
			data.telemStruct.steerAngle = steeringPID.input;
			//data.telemStruct.orient[0] = orZ[0];
			//data.telemStruct.orient[1] = orZ[1];
			//data.telemStruct.orient[2] = orZ[2];
			telemSaveData(&data); 
			samplesaved = 1;
		}
	}
	telemSkip = (telemSkip + 1) & ~SKIP_COUNT;
	return samplesaved;
}

void telemErase(unsigned long numSamples){
	dfMemEraseSectorsForSamples(numSamples, sizeof(telemU));
}
