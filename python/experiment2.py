#!/usr/bin/env python
"""
authors: stanbaek, apullin

"""
import numpy as np
from lib import command
from struct import *
import time,sys
from xbee import XBee
import serial
import glob
from math import ceil,floor
from callbackFunc import xbee_received
import shared
import datetime


DEST_ADDR = '\x20\x52'
imudata_file_name = 'imudata.txt'
#statedata_file_name = 'statedata.txt'
#dutycycle_file_name = 'dutycycle.txt'

###
MOVE_SEG_CONSTANT = 0
MOVE_SEG_RAMP = 1
MOVE_SEG_SIN = 2
MOVE_SEG_TRI = 3
MOVE_SEG_SAW = 4
MOVE_SEG_IDLE = 5

##
STEER_MODE_DECREASE = 0
STEER_MODE_INCREASE = 1
STEER_MODE_SPLIT = 2

try:
    ser = serial.Serial(shared.BS_COMPORT, shared.BS_BAUDRATE, \
                    timeout=3, rtscts=0)
except serial.serialutil.SerialException:
    print "Could not open serial port:",shared.BS_COMPORT
    sys.exit()

xb = XBee(ser, callback = xbee_received)

###### Operation Flags ####
SAVE_DATA = True
RESET_ROBOT = True


########## Helper functions #################
def xb_send(status, type, data):
    payload = chr(status) + chr(type) + ''.join(data)
    xb.tx(dest_addr = DEST_ADDR, data = payload)

def resetRobot():
    xb_send(0, command.SOFTWARE_RESET, pack('h',0))

def findFileName():
    filenames = glob.glob("*imudata*.txt");
    # Explicitly remove "imudata.txt", since that can mess up the pattern
    if 'imudata.txt' in filenames:
        filenames.remove('imudata.txt')

    if filenames == []:
        dataFileName = "imudata1.txt"
    else:
        filenames.sort()
        filenum = [int(fn[7:-4]) for fn in filenames]
        filenum.sort()
        filenum = filenum[-1] + 1
        dataFileName = "imudata" + str(filenum) + ".txt"
    return dataFileName

def writeFileHeader(dataFileName):
    global angRateDeg, angRate, motorgains, steeringGains, runtime, \
            n, moveq
    now = datetime.datetime.now()

    fileout = open(dataFileName,'w')
    #write out parameters
    fileout.write('% ' + now.strftime("%m/%d/%Y %H:%M") + '\n')
    fileout.write('%  Comments: \n')
    fileout.write('%  angrate       = ' + str(angRateDeg) + ' deg\n')
    fileout.write('%                = ' + str(angRate) + ' raw\n')
    fileout.write('%  motorgains    = ' + repr(motorgains) + '\n')
    fileout.write('%  steeringGains = ' + repr(steeringGains) + '\n')
    fileout.write('%  runtime       = ' + repr(runtime) + '\n')
    fileout.write('%  n             = ' + repr(n) + '\n')
    fileout.write('%  moveq       = ' + repr(moveq) + '\n')
    fileout.write('% Columns: \n')
    fileout.write('% time | Llegs | Rlegs | DCL | DCR | GyroX | GyroY | GyroZ | GryoZAvg | AccelX | AccelY |AccelZ | LBEMF | RBEMF | SteerOut | Vbatt | SteerAngle\n')
    fileout.close()

def dlProgress(current, total):
    percent = int(100.0*current/total)
    dashes = int(floor(percent/100.0 * 45))
    stars = 45 - dashes - 1
    barstring = '|' + '-'*dashes + '>' + '*'*stars + '|'
    #sys.stdout.write("\r" + "Downloading ...%d%%   " % percent)
    sys.stdout.write("\r" + str(current).rjust(5) +"/"+ str(total).ljust(5) + "   ")
    sys.stdout.write(barstring)
    sys.stdout.flush()
########## END Helper functions ########################


def main():
    global angRateDeg, angRate, motorgains, steeringGains, runtime, \
            n, moveq

    if SAVE_DATA:
        dataFileName = findFileName();
        print "Data file:  ", dataFileName

    #tic = time.time()
    #xb_send(0, command.ECHO,  "abcdefgh")

    if RESET_ROBOT:
        print "Resetting robot..."
        resetRobot()
        time.sleep(1)

    #~ shared.awake = 0;
    #~ while not(shared.awake):
        #~ print "Waking robot ... "
        #~ xb_send(0, command.SLEEP, pack('b',0))
        #~ time.sleep(0.2)

    #~ time.sleep(1)
    #~ print "Sleeping robot ... "
    #~ xb_send(0, command.SLEEP, pack('b',1))
    #~ time.sleep(1)

    #~ xb.halt()
    #~ ser.close()
    #~ sys.exit()

    angRateDeg = 0
    angRate = round( angRateDeg / shared.count2deg)
    while not(shared.steering_rate_set):
        print "Setting steering rate..."
        xb_send(0, command.SET_CTRLD_TURN_RATE, pack('h',angRate))
        time.sleep(0.25)

    motorgains = [15000,500,150,0,0 , 15000,500,150,0,0] #Hardware PID
    #motorgains = [200,2,0,2,0,    200,2,0,2,0]  #Software PID
    while not(shared.motor_gains_set):
        print "Setting motor gains..."
        xb_send(0, command.SET_PID_GAINS, pack('10h',*motorgains))
        time.sleep(0.25)

    steeringGains = [0,0,0,0,0,  1]
    #steeringGains = [5,1,0,1,0,  1]
    #steeringGains = [20,1,0,1,0,  STEER_MODE_SPLIT]
    while not (shared.steering_gains_set):
        print "Setting steering gains..."
        xb_send(0, command.SET_STEERING_GAINS, pack('6h',*steeringGains))
        time.sleep(0.25)

    #Constant example
    #moves = 2
    #moveq = [moves, \
    #         150, 150, 5000,   MOVE_SEG_CONSTANT, 0, 0, 0,
    #        150, 150, 5000,   MOVE_SEG_CONSTANT, 0, 0, 0]

    #Ramp example
    moves = 3
    moveq = [moves, \
        0,   0,   2000,   MOVE_SEG_RAMP,    100, 100, 0,
        200, 200, 3000,   MOVE_SEG_CONSTANT, 0,  0,  0,
        200, 200, 2000,   MOVE_SEG_RAMP, -100,  -100,  0]

    #Sin example
    #RAD_TO_BAMS16 = (0x7FFF)/(3.1415)
    #phase = 3.1415/2 * RAD_TO_BAMS16
    #moves = 2
    #moveq = [moves, \
    #         76,   76,   2000,   MOVE_SEG_SIN,  75, 1000, phase,
#	     75, 75, 2000,   MOVE_SEG_CONSTANT, 0,  0,  0]
    

    times = sum([moveq[i] for i in [ind*7+3 for ind in range(0,moveq[0])]])


    #runtime = 1000; #in milliseconds, whole numbers only
    runtime = times
    leadinTime = 500;
    leadoutTime = 500;
    #calculate the number of telemetry packets we expect

    n = int(ceil(150 * (runtime + leadinTime + leadoutTime) / 1000.0))
    #allocate an array to write the downloaded telemetry data into
    shared.imudata = [ [] ] * n
    print "Samples: ",n

    time.sleep(0.25)

    eraseStartTime = time.time()
    if SAVE_DATA:
        xb_send(0, command.ERASE_SECTORS, pack('L',n))
        print "started flash erase ...",
        while not (shared.flash_erased):
            time.sleep(0.25)
            sys.stdout.write('.')
            if (time.time() - eraseStartTime) > 8:
                print"\nFlash erase timeout, retrying;"
                xb_send(0, command.ERASE_SECTORS, pack('L',n))
                eraseStartTime = time.time()
        print "\nFlash erase done."

    raw_input("Press enter to start run ...")
    time.sleep(0.5)




    if SAVE_DATA:
        print "started save"
        xb_send(0, command.SPECIAL_TELEMETRY, pack('L',n))
        time.sleep(leadinTime / 1000.0)

    xb_send(0, command.SET_MOVE_QUEUE, pack('=h'+moves*'hhLhhhh', *moveq))
    ###This section only for explict closed loop thrust commands
    #thrust = [f, runtime, f, runtime, 0]
    #xb_send(0, command.SET_THRUST_CLOSED_LOOP, pack('5h',*thrust))


    if SAVE_DATA:
        #time.sleep((1/300.)*n + 3) #300Hz sampling, 20% overage
        time.sleep((runtime + leadinTime + leadoutTime)/1000.0 + 1)
        raw_input("Press any key to start readback ...")
        print "started readback"
        xb_send(0, command.FLASH_READBACK, pack('=L',n))

        # While waiting, write parameters to start of file
        writeFileHeader(dataFileName)

        #time.sleep(delay*n + 3)
        #while shared.pkts != n:
        #    print "Retry"
        #    shared.imudata = [ [] ] * n
        #    shared.pkts = 0
        #    xb_send(0, command.FLASH_READBACK, pack('=h',n))
        #    time.sleep(delay*n + 3)
        #    if shared.pkts > n:
        #        print "too many packets"
        #        break
        dlStart = time.time()
        shared.last_packet_time = dlStart
        shared.bytesIn = 0
        while shared.imudata.count([]) > 0:
            time.sleep(0.1)
            #print "Downloading ...",(n-shared.imudata.count([])),"/",n
            dlProgress(n-shared.imudata.count([]) , n)
            if (time.time() - shared.last_packet_time) > shared.readback_timeout:
                print "\nReadback timeout exceeded, restarting."
                raw_input("Press any key to start readback ...")
                shared.imudata = [ [] ] * n
                print "started readback"
                dlStart = time.time()
                shared.last_packet_time = dlStart
                xb_send(0, command.FLASH_READBACK, pack('=L',n))

        dlEnd = time.time()
        dlProgress(n-shared.imudata.count([]) , n)
        print "\nTime: %.2f s ,  %.3f KBps" % ( (dlEnd - dlStart), \
                            shared.bytesIn / (1000*(dlEnd - dlStart)))


        print "readback done"
        fileout = open(dataFileName, 'a')
        np.savetxt(fileout , np.array(shared.imudata), '%d', delimiter = ',')
        #np.savetxt(fileout , np.array(flat_imudata), '%d', delimiter = ',')

        print "data saved to ",dataFileName


    print "Ctrl + C to exit"

    while True:
        try:
            time.sleep(1)
            #print ".",
        except KeyboardInterrupt:
            break

    xb.halt()
    ser.close()


    print "Done"

#Provide a try-except over the whole main function
# for clean exit. The Xbee module should have better
# provisions for handling a clean exit, but it doesn't.
if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print "\nRecieved Ctrl+C, exiting."
        xb.halt()
        ser.close()
    except Exception as args:
        print "\nGeneral exception:",args
        print "Attemping to exit cleanly..."
        xb.halt()
        ser.close()
        sys.exit()
    except serial.serialutil.SerialException:
        xb.halt()
        ser.close()
