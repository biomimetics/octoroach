from or_helpers import *
from math import ceil

#moveq     = np.zeros((trials,moves,3))


MOVES_PER_PKT = 4

moves = 9
moveq = [moves, \
    0,   0,   500,   MOVE_SEG_RAMP,    300, 300, 0,
    150, 150, 5000,   MOVE_SEG_CONSTANT, 0,  0,  0,
    150, 150, 500,   MOVE_SEG_RAMP, -300,  -300,  0,
    0,   0,   500,   MOVE_SEG_RAMP,    300, 300, 0,
    150, 150, 5000,   MOVE_SEG_CONSTANT, 0,  0,  0,
    150, 150, 500,   MOVE_SEG_RAMP, -300,  -300,  0,
    0,   0,   500,   MOVE_SEG_RAMP,    300, 300, 0,
    150, 150, 5000,   MOVE_SEG_CONSTANT, 0,  0,  0,
    150, 150, 500,   MOVE_SEG_RAMP, -300,  -300,  0]


npmoves = np.array(moveq[1:])

npmoves = npmoves.reshape(moves,7)

print npmoves

numPkts =  int(ceil(float(moves)/float(MOVES_PER_PKT)))

pkts = npmoves.reshape(

for q in range(numPkts):
    print "pkt"

#
# Fill in the moveq however you want...
#

# Send move queue
#for q in range(queues):
#   mq = moveq[moves_max*q:moves_max*(q+1)]
   #xb_send(0, command.SET_MOVE_QUEUE, pack('=h'+mq.shape[0]*'hhL', \
   #                *([mq.shape[0]] +  list(mq.reshape(mq.size)))))
#   print mq

