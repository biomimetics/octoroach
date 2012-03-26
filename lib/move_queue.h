#ifndef __MOVE_QUEUE_H
#define __MOVE_QUEUE_H

#include "queue.h"
#include "pid.h"

enum moveSegT{
	MOVE_SEG_CONSTANT,
	MOVE_SEG_RAMP,
	MOVE_SEG_SIN,
	MOVE_SEG_TRI,
	MOVE_SEG_SAW,
	MOVE_SEG_IDLE
};

typedef struct
{
	int inputL, inputR;
	unsigned long duration;
	enum moveSegT type;
	int params[3];
} moveCmdStruct;

typedef moveCmdStruct* moveCmdT;

//typedef generic pointer type, Item;
typedef Queue MoveQueue;

MoveQueue mqInit(int max_size);

void mqPush(MoveQueue mq, moveCmdT mv);

moveCmdT mqPop(MoveQueue queue);

int mqIsFull(MoveQueue queue);

int mqIsEmpty(MoveQueue queue);

int mqGetSize(MoveQueue queue);

#endif // __MOVE_QUEUE_H
