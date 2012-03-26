/* Queue (FIFO) for moveCmdT

 */

#include "queue.h"
#include "move_queue.h"
#include "payload.h"
#include "pid.h"
#include "p33Fxxxx.h"
#include <stdio.h>      // for NULL
#include <stdlib.h>     // for malloc


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
    return (moveCmdT)queuePop(queue);
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

