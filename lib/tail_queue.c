// Contents of this file are copyright Andrew Pullin, 2013

#include "tail_queue.h"
#include "pid.h"
#include "p33Fxxxx.h"
#include <stdio.h>      // for NULL
#include <stdlib.h>     // for malloc


/*-----------------------------------------------------------------------------
 *          Public functions
-----------------------------------------------------------------------------*/

TailQueue tailqInit(int max_size) {
    Queue tq = queueInit(max_size);
    return tq;
}

void tailqPush(TailQueue tq, tailCmdT newItem) {

    tailCmdT temp;

    if (queueIsFull(tq)) {
        temp = (tailCmdT)queuePop(tq);
        free(temp);
    }

    queueAppend(tq, newItem);

}

tailCmdT tailqPop(TailQueue queue) {
    return (tailCmdT)queuePop(queue);
}

int tailqIsFull(TailQueue queue) {
    return queueIsFull(queue);
}

int tailqIsEmpty(TailQueue queue) {
    return queueIsEmpty(queue);
}


int tailqGetSize(TailQueue queue) {
    return queueGetSize(queue);
}

