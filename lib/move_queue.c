/* Queue (FIFO) for moveCmdT

 */

#include "queue.h"
#include "move_queue.h"
#include "payload.h"
#include "pid.h"
#include "p33Fxxxx.h"
//#include <stdio.h>      // for NULL
#include <stdlib.h>     // for malloc

static int moveQueueLooping = 0;

/*-----------------------------------------------------------------------------
 *          Public functions
-----------------------------------------------------------------------------*/

MoveQueue mqInit(int max_size) {
    Queue mq = queueInit(max_size);
    return mq;
}

void mqPush(MoveQueue mq, moveCmdT mv) {

    moveCmdT move;

    if (queueIsFull(mq)) {
        move = (moveCmdT)queuePop(mq);
        free(move);
    }

    queueAppend(mq, mv);

}

moveCmdT mqPop(MoveQueue queue) {
    moveCmdT thismove = (moveCmdT)queuePop(queue);
    if(moveQueueLooping){
        queuePush(queue, thismove);
    }
    //return (moveCmdT)queuePop(queue);
    return thismove;
}

int mqIsFull(MoveQueue queue) {
    return queueIsFull(queue);
}

int mqIsEmpty(MoveQueue queue) {
    return queueIsEmpty(queue);
}


int mqGetSize(MoveQueue queue) {
    return queueGetSize(queue);
}

void mqLoopingOnOff(int onoff){
    moveQueueLooping = onoff;
}

