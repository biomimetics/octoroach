#ifndef __TAIL_QUEUE_H
#define __TAIL_QUEUE_H

#include "queue.h"
//#include "pid.h"

enum tailSegT{
	TAIL_SEG_CONSTANT,
	TAIL_SEG_RAMP,
	TAIL_SEG_SIN,
	TAIL_SEG_TRI,
	TAIL_SEG_SAW,
	TAIL_SEG_IDLE,
	TAIL_GYRO_CONTROL
};

typedef struct
{
    float angle; 
    unsigned long duration;
    enum tailSegT type;
    int params[3];
} tailCmdStruct;

typedef tailCmdStruct* tailCmdT;

//typedef generic pointer type, Item;
typedef Queue TailQueue;

TailQueue tailqInit(int );

void tailqPush(TailQueue , tailCmdT );

tailCmdT tailqPop(TailQueue );

int tailqIsFull(TailQueue );

int tailqIsEmpty(TailQueue );

int tailqGetSize(TailQueue );

#endif // __TAIL_QUEUE_H
