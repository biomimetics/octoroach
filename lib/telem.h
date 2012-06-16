#ifndef __DFMEM_EXTRA_H
#define __DFMEM_EXTRA_H

//Telemetry packet structure
//This is specific to apullin's OctoROACH code
typedef struct {
		unsigned long timeStamp; 
		int inputL;  
		int inputR;
		int dcL;
		int dcR;
		int gyroX;
		int gyroY;
		int gyroZ;
		int gyroAvg;
		int accelX;
		int accelY;
		int accelZ;
		int bemfL;
		int bemfR;
		int sOut;
		int Vbatt;
		int steerAngle;	
		//float orient[3];
	} telemStruct_t;

typedef union packedTelemUnion {
	telemStruct_t telemStruct;
	unsigned char dataArray[sizeof(telemStruct_t)];
} telemU;

#define PACKETSIZE sizeof(telemStruct_t)
#define PKT_INDEX_SIZE 4 //for sending a 4-byte (ulong) telemetry packet index

// Prototypes
void telemSetup(); //To be called in main
void telemReadbackSamples(unsigned long);
void telemSendDataDelay(unsigned char, unsigned char*, int delaytime_ms);
void telemSaveData(telemU *data);
void telemSetSavesToSave(unsigned long n);
void telemErase(unsigned long);

#endif  // __DFMEM_EXTRA_H
