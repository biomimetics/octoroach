#ifndef __DFMEM_EXTRA_H
#define __DFMEM_EXTRA_H

//#include <dfmem.h>

// AT45 DataFlash Memory geometry
#if (defined(__DFMEM_8MBIT))
	#define MAXSECTOR 16
	#define MAXPAGES 4096
	#define BUFFERSIZE 264
	#define BYTES_PER_PAGE 264
	#define PAGES_PER_BLOCK 8
	#define BLOCKS_PER_SECTOR 32
#elif (defined(__DFMEM_16MBIT))
	#define MAXSECTOR 16
	#define MAXPAGES 4096
	#define BUFFERSIZE 528
	#define BYTES_PER_PAGE 528
	#define PAGES_PER_BLOCK 8
	#define BLOCKS_PER_SECTOR 32
#elif (defined(__DFMEM_32MBIT))
	#define MAXSECTOR 64
	#define MAXPAGES 8192
	#define BUFFERSIZE 528
	#define BYTES_PER_PAGE 528
	#define PAGES_PER_BLOCK 8
	#define BLOCKS_PER_SECTOR 64
#elif (defined(__DFMEM_64MBIT))
	#define MAXSECTOR 32
	#define MAXPAGES 8192
	#define BUFFERSIZE 1056
	#define BYTES_PER_PAGE 1056
	#define PAGES_PER_BLOCK 8
	#define BLOCKS_PER_SECTOR 32
#else
    #error "You need to specify the size of your memory chip by defining one of the following \
			flags: __DFMEM_8MBIT, __DFMEM_16MBIT, or __DFMEM_32MBIT."
#endif

//Telemetry packet structure
//This is specific to Andrew P's OctoROACH code
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
	} telemStruct_t;

typedef union packedTelemUnion {
	telemStruct_t telemStruct;
	unsigned char dataArray[sizeof(telemStruct_t)];
} telemU;

#define PACKETSIZE sizeof(telemStruct_t)
#define PKT_INDEX_SIZE 4 //for sending a 4-byte (ulong) telemetry packet index

// Prototypes
void eraseDFMemSectors0a0b(void);
void eraseDFMemSectorsForSamples(unsigned int);
void readDFMemBySample(unsigned int);
void sendDataDelay(unsigned char, unsigned char*, int delaytime_ms);
void saveTelemData(telemU *data);


#endif  // __DFMEM_EXTRA_H
