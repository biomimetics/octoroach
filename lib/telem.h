// Contents of this file are copyright Andrew Pullin, 2013

#ifndef __TELEM_H
#define __TELEM_H

//Telemetry packet structure

typedef union packedTelemUnion {
    unsigned long sampleIndex;
    unsigned long timestamp;
    unsigned char* telemData;
} telemStruct_t;

#define TELEM_STREAM_OFF  0
#define TELEM_STREAM_ON   1

// Prototypes
void telemSetup(); //To be called in main
void telemReadbackSamples(unsigned long);
void telemSendDataDelay(int delaytime_ms);
void telemSaveData(telemStruct_t *data);
void telemSetSamplesToSave(unsigned long n);
void telemErase(unsigned long);
void telemSetSkip(unsigned int skipnum);
void telemSetStartTime(void);

#endif  // __TELEM_H
