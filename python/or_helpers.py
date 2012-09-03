import glob
import time
import sys
from lib import command
from callbackFunc import xbee_received
import datetime
import serial
import shared
from struct import pack
from xbee import XBee
from math import ceil,floor
import numpy as np

## Constants
###
MOVE_SEG_CONSTANT = 0
MOVE_SEG_RAMP = 1
MOVE_SEG_SIN = 2
MOVE_SEG_TRI = 3
MOVE_SEG_SAW = 4
MOVE_SEG_IDLE = 5
MOVE_SEG_LOOP_DECL = 6
MOVE_SEG_LOOP_CLEAR = 7
MOVE_SEG_QFLUSH = 8

###
TAIL_SEG_CONSTANT = 0
TAIL_SEG_RAMP = 1
TAIL_SEG_SIN = 2
TAIL_SEG_TRI = 3
TAIL_SEG_SAW = 4
TAIL_SEG_IDLE = 5
TAIL_GYRO_CONTROL = 6

##
STEER_MODE_OFF = 0
STEER_MODE_INCREASE = 1
STEER_MODE_DECREASE = 2
STEER_MODE_SPLIT = 3



########## Helper functions #################
def xb_send(xb, DEST_ADDR, status, type, data):
    payload = chr(status) + chr(type) + ''.join(data)
    xb.tx(dest_addr = DEST_ADDR, data = payload)
    
def xb_safe_exit():
    print "Halting xb"
    shared.xb.halt()
    print "Closing serial"
    shared.ser.close()
    print "Exiting..."
    sys.exit(1)

def resetRobot():
    xb_send(shared.xb, shared.DEST_ADDR, 0, command.SOFTWARE_RESET, pack('h',1))

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
    now = datetime.datetime.now()

    fileout = open(dataFileName,'w')
    #write out parameters
    fileout.write('% ' + now.strftime("%m/%d/%Y %H:%M") + '\n')
    fileout.write('%  Comments: \n')
    fileout.write('%  angrate (deg) = ' + str(shared.angRateDeg) + '\n')
    fileout.write('%  angrate (raw) = ' + str(shared.angRate) + '\n')
    fileout.write('%  motorgains    = ' + repr(shared.motorGains) + '\n')
    fileout.write('%  steeringGains = ' + repr(shared.steeringGains) + '\n')
    fileout.write('%  runtime       = ' + repr(shared.runtime) + '\n')
    fileout.write('%  numSamples    = ' + repr(shared.numSamples) + '\n')
    fileout.write('%  moveq         = ' + repr(shared.moveq) + '\n')
    fileout.write('% Columns: \n')
    fileout.write('% time | Llegs | Rlegs | DCL | DCR | GyroX | GyroY | GyroZ | GryoZAvg | AccelX | AccelY |AccelZ | LBEMF | RBEMF | SteerOut | Vbatt | SteerAngle | Tail Angle\n')
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
    
    
def sendEcho(msg):
    xb_send(shared.xb, shared.DEST_ADDR, 0, command.ECHO, msg)
    
def downloadTelemetry(numSamples):
    #Wait for run length before starting download
    time.sleep((shared.runtime + shared.leadinTime + shared.leadoutTime)/1000.0 + 1)
    
    raw_input("Press Enter to start readback ...")
    print "started readback"
    xb_send(shared.xb, shared.DEST_ADDR, 0, command.FLASH_READBACK, pack('=L',numSamples))

    # While downloading via callbackfunc, write parameters to start of file
    writeFileHeader(shared.dataFileName)
        
    dlStart = time.time()
    shared.last_packet_time = dlStart
    shared.bytesIn = 0
    while shared.imudata.count([]) > 0:
        time.sleep(0.1)
        #print "Downloading ...",(n-shared.imudata.count([])),"/",n
        dlProgress(numSamples -shared.imudata.count([]) , numSamples)
        if (time.time() - shared.last_packet_time) > shared.readback_timeout:
            print "\nReadback timeout exceeded, restarting."
            raw_input("Press Enter to start readback ...")
            shared.imudata = [ [] ] * numSamples
            print "started readback"
            dlStart = time.time()
            shared.last_packet_time = dlStart
            xb_send(shared.xb, shared.DEST_ADDR, \
                    0, command.FLASH_READBACK, pack('=L',numSamples))

    dlEnd = time.time()
    #Final update to download progress bar to make it show 100%
    dlProgress(numSamples-shared.imudata.count([]) , numSamples)
    print "\nTime: %.2f s ,  %.3f KBps" % ( (dlEnd - dlStart), \
                        shared.bytesIn / (1000*(dlEnd - dlStart)))

    print "readback done"
    fileout = open(shared.dataFileName, 'a')
    np.savetxt(fileout , np.array(shared.imudata), '%d,'*14+'%f,%d,%d,%f,%f,%d,%d,%d', delimiter = ',')

    print "data saved to ",shared.dataFileName
    #Done with flash download and save
    
    
def wakeRobot():
    shared.awake = 0;
    while not(shared.awake):
        print "Waking robot ... "
        xb_send(shared.xb, shared.DEST_ADDR, 0, command.SLEEP, pack('b',0))
        time.sleep(0.2)
        
        
def sleepRobot():
    print "Sleeping robot ... "
    xb_send(shared.xb, shared.DEST_ADDR, 0, command.SLEEP, pack('b',1))
    
def setSteeringRate(rate):
    count = 1
    shared.angRateDeg = rate
    shared.angRate = round( shared.angRateDeg / shared.count2deg)
    while not(shared.steering_rate_set):
        print "Setting steering rate...   ",count,"/8"
        count = count + 1
        xb_send(shared.xb, shared.DEST_ADDR, \
                0, command.SET_CTRLD_TURN_RATE, pack('h',shared.angRate))
        time.sleep(0.3)
        if count > 8:
            print "Unable to set steering rate, exiting."
            xb_safe_exit()

def setMotorGains(gains):
    count = 1
    shared.motorGains = gains
    while not(shared.motor_gains_set):
        print "Setting motor gains...   ",count,"/8"
        xb_send(shared.xb, shared.DEST_ADDR, \
                0, command.SET_PID_GAINS, pack('10h',*gains))
        time.sleep(0.3)
        count = count+1
        if count > 8:
            print "Unable to set motor gains, exiting."
            xb_safe_exit()
    
def setSteeringGains(gains):
    count = 1
    shared.steeringGains = gains
    while not (shared.steering_gains_set):
        print "Setting steering gains...   ",count,"/8"
        xb_send(shared.xb, shared.DEST_ADDR, \
                0, command.SET_STEERING_GAINS, pack('6h',*gains))
        time.sleep(0.3)
        count = count+1
        if count > 8:
            print "Unable to set steering gains, exiting."
            xb_safe_exit()

def setTailGains(gains):
    count = 1
    shared.steeringGains = gains
    while not (shared.tail_gains_set):
        print "Setting tail gains...   ",count,"/8"
        xb_send(shared.xb, shared.DEST_ADDR, \
                0, command.SET_TAIL_GAINS, pack('5h',*gains))
        time.sleep(0.3)
        count = count+1
        if count > 8:
            print "Unable to set tail gains, exiting."
            xb_safe_exit()
        

def eraseFlashMem(numSamples):
    eraseStartTime = time.time()
    xb_send(shared.xb, shared.DEST_ADDR, \
            0, command.ERASE_SECTORS, pack('L',numSamples))
    print "started flash erase ...",
    while not (shared.flash_erased):
        time.sleep(0.25)
        sys.stdout.write('.')
        if (time.time() - eraseStartTime) > 8:
            print"\nFlash erase timeout, retrying;"
            xb_send(shared.xb, shared.DEST_ADDR,0, command.ERASE_SECTORS, pack('L',numSamples))
            eraseStartTime = time.time()
    print "\nFlash erase done."
    
def startTelemetrySave(numSamples):
    shared.numSamples = numSamples
    print "started save"
    xb_send(shared.xb, shared.DEST_ADDR, \
            0, command.SPECIAL_TELEMETRY, pack('L',numSamples))
    
def sendMoveQueue(moveq):
    shared.moveq = moveq
    nummoves = moveq[0]
    #xb_send(shared.xb, shared.DEST_ADDR, \
    #        0, command.SET_MOVE_QUEUE, pack('=h'+nummoves*'hhLhhhh', *moveq))
    xb_send(shared.xb, shared.DEST_ADDR, \
            0, command.SET_MOVE_QUEUE, pack('=h'+nummoves*'hhLhhhhhh', *moveq))

def sendTailQueue(tailq):
    shared.tailq = tailq
    nummoves = tailq[0]
    xb_send(shared.xb, shared.DEST_ADDR, \
            0, command.SET_TAIL_QUEUE, pack('=h'+nummoves*'fLhhhh', *tailq))

def setMotorSpeeds(spleft, spright):
    thrust = [spleft, 0, spright, 0, 0]
    xb_send(shared.xb, shared.DEST_ADDR, \
            0, command.SET_THRUST_CLOSED_LOOP, pack('5h',*thrust))
    
def setupSerial():
    print "Setting up serial ..."
    try:
        shared.ser = serial.Serial(shared.BS_COMPORT, shared.BS_BAUDRATE, \
                    timeout=3, rtscts=1)
    except serial.serialutil.SerialException:
        print "Could not open serial port:",shared.BS_COMPORT
        sys.exit()
    
    shared.xb = XBee(shared.ser, callback = xbee_received)
        
        
def calcNumSamples(moveq):
    #Calculates the total movement time from the move queue above
    shared.runtime = sum([moveq[i] for i in [ind*9+3 for ind in range(0,moveq[0])]])
   
    #calculate the number of telemetry packets we expect
    n = int(ceil(150 * (shared.runtime + shared.leadinTime + shared.leadoutTime) / 1000.0))
    #allocate an array to write the downloaded telemetry data into
    shared.imudata = [ [] ] * n
    print "Samples: ",n
    return n
