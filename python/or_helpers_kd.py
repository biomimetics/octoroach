import glob
import time
import sys
from lib import command
from callbackFunc_kd import xbee_received
import datetime
import serial
import shared_kd as shared
from struct import pack,unpack
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

##
STEER_MODE_DECREASE = 0
STEER_MODE_INCREASE = 1
STEER_MODE_SPLIT = 2


class Robot:
    motor_gains_set = False
    steering_gains_set = False
    steering_rate_set = False
    robot_queried = False
    flash_erased = False
    robot_awake = True
    motorGains = [0,0,0,0,0, 0,0,0,0,0]
    steeringGains = [0,0,0,0,0]
    angRateDeg = 0;
    angRate = 0;
    
    def __init__(self, address, xb):
        self.DEST_ADDR = address
        self.DEST_ADDR_int = unpack('>h',self.DEST_ADDR)[0] #address as integer
        self.xb = xb
        print "Robot DEST_ADDR_int = 0x%02X " % self.DEST_ADDR_int
    
    def clAnnounce(self):
        print "DST: 0x%02X | " % self.DEST_ADDR_int,
    
    def tx(self, status, type, data):
        payload = chr(status) + chr(type) + ''.join(data)
        self.xb.tx(dest_addr = self.DEST_ADDR, data = payload)
        
    def reset(self):
        self.clAnnounce()
        print "Resetting robot..."
        self.tx( 0, command.SOFTWARE_RESET, pack('h',1))
        
    def sendEcho(self, msg):
        self.tx( 0, command.ECHO, msg)
    
    def wake(self, period = 0.2):
        self.robot_awake = False;
        while not(self.robot_awake):
            self.clAnnounce()
            print "Waking robot ... "
            self.tx(0, command.SLEEP, pack('b',0))
            time.sleep(period)
        
        
    def sleep(self):
        self.clAnnounce()
        print "Sleeping robot ... "
        self.tx( 0, command.SLEEP, pack('b',1))
        
    def setSteeringRate(self, rate, retries = 8):
        tries = 1
        self.angRateDeg = rate
        self.angRate = round( self.angRateDeg * 14.375)
        while not(self.steering_rate_set) and (tries <= retries):
            self.clAnnounce()
            print "Setting steering rate...   ",tries,"/",retries
            tries = tries + 1
            self.tx( 0, command.SET_CTRLD_TURN_RATE, pack('h', self.angRate))
            time.sleep(0.3)

    def setMotorGains(self, gains):
        tries = 1
        self.motorGains = gains
        while not(self.motor_gains_set):
            self.clAnnounce()
            print "Setting motor gains...   ",tries,"/8"
            self.tx( 0, command.SET_PID_GAINS, pack('10h',*gains))
            time.sleep(0.3)
            if count > 8:
                self.clAnnounce()
                print "Unable to set motor gains, exiting."
                xb_safe_exit()
        
    def setSteeringGains(self, gains):
        tries = 1
        self.steeringGains = gains
        while not (self.steering_gains_set):
            self.clAnnounce()
            print "Setting steering gains...   ",tries,"/8"
            self.tx( 0, command.SET_STEERING_GAINS, pack('6h',*gains))
            time.sleep(0.3)
            if count > 8:
                self.clAnnounce()
                print "Unable to set steering gains, exiting."
                xb_safe_exit()

    def eraseFlashMem(self, numSamples):
        eraseStartTime = time.time()
        self.tx( 0, command.ERASE_SECTORS, pack('L',numSamples))
        self.clAnnounce()
        print "started flash erase ...",
        while not (self.flash_erased):
            time.sleep(0.25)
            sys.stdout.write('.')
            if (time.time() - eraseStartTime) > 8:
                print"\nFlash erase timeout, retrying;"
                self.tx( 0, command.ERASE_SECTORS, pack('L',numSamples))
                eraseStartTime = time.time()
        self.clAnnounce()
        print "\nFlash erase done."
        
    def startTelemetrySave(self, numSamples):
        self.numSamples = numSamples
        clAnnounce
        print "started save"
        self.tx( 0, command.SPECIAL_TELEMETRY, pack('L',numSamples))
        
    def sendMoveQueue(self, moveq):
        self.moveq = moveq
        self.nummoves = moveq[0]
        self.tx( 0, command.SET_MOVE_QUEUE, pack('=h'+nummoves*'hhLhhhh', *moveq))
        
    def setMotorSpeeds(self, spleft, spright):
        thrust = [spleft, 0, spright, 0, 0]
        self.tx( 0, command.SET_THRUST_CLOSED_LOOP, pack('5h',*thrust))
            
    def query(self, retries = 8):
        self.robot_queried = False
        tries = 1
        while not(self.robot_queried) and (tries <= retries):
            self.clAnnounce()
            print "Querying robot , ",tries,"/",retries
            self.tx( 0,  command.WHO_AM_I, "Robot Echo") #sent text is unimportant
            tries = tries + 1
            time.sleep(0.3)   
        


########## Helper functions #################


def setupSerial(COMPORT , BAUDRATE , timeout = 3, rtscts = 0):
    print "Setting up serial ..."
    try:
        shared.ser = serial.Serial(port = COMPORT, baudrate = BAUDRATE, \
                    timeout=timeout, rtscts=rtscts)
    except serial.serialutil.SerialException:
        print "Could not open serial port:",shared.BS_COMPORT
        sys.exit()
    
    return XBee(shared.ser, callback = xbee_received)
    
    

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

def resetRobot(DEST_ADDR):
    xb_send(shared.xb, DEST_ADDR, 0, command.SOFTWARE_RESET, pack('h',1))

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
    
    
def sendEcho(msg):
    xb_send(shared.xb, shared.DEST_ADDR, 0, command.ECHO, msg)
    
def downloadTelemetry(numSamples,RobName):
    #Wait for run length before starting download
    time.sleep((shared.runtime + shared.leadinTime + shared.leadoutTime)/1000.0 + 1)
    
    raw_input("Press Enter to start readback ...")
    print "started readback"
    xb_send(shared.xb, RobName, 0, command.FLASH_READBACK, pack('=L',numSamples))

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
            xb_send(shared.xb, RobName, \
                    0, command.FLASH_READBACK, pack('=L',numSamples))

    dlEnd = time.time()
    #Final update to download progress bar to make it show 100%
    dlProgress(numSamples-shared.imudata.count([]) , numSamples)
    print "\nTime: %.2f s ,  %.3f KBps" % ( (dlEnd - dlStart), \
                        shared.bytesIn / (1000*(dlEnd - dlStart)))

    print "readback done"
    fileout = open(shared.dataFileName, 'a')
    np.savetxt(fileout , np.array(shared.imudata), '%d', delimiter = ',')

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
    
def setSteeringRate(rate,RobName):
    count = 1
    shared.angRateDeg = rate
    shared.angRate = round( shared.angRateDeg / shared.count2deg)
    while not(shared.steering_rate_set):
        print "Setting steering rate...   ",count,"/8"
        count = count + 1
        xb_send(shared.xb, RobName, \
                0, command.SET_CTRLD_TURN_RATE, pack('h',shared.angRate))
        time.sleep(0.3)
        if count > 8:
            print "Unable to set steering rate, exiting."
            xb_safe_exit()

def setMotorGains(gains,RobName):
    count = 1
    shared.motorGains = gains
    while not(shared.motor_gains_set):
        print "Setting motor gains...   ",count,"/8"
        xb_send(shared.xb, RobName, \
                0, command.SET_PID_GAINS, pack('10h',*gains))
        time.sleep(0.3)
        if count > 8:
            print "Unable to set motor gains, exiting."
            xb_safe_exit()
    
def setSteeringGains(gains,RobName):
    count = 1
    shared.steeringGains = gains
    while not (shared.steering_gains_set):
        print "Setting steering gains...   ",count,"/8"
        xb_send(shared.xb, RobName, \
                0, command.SET_STEERING_GAINS, pack('6h',*gains))
        time.sleep(0.3)
        if count > 8:
            print "Unable to set steering gains, exiting."
            xb_safe_exit()

def eraseFlashMem(numSamples,RobName):
    eraseStartTime = time.time()
    xb_send(shared.xb, RobName, \
            0, command.ERASE_SECTORS, pack('L',numSamples))
    print "started flash erase ...",
    while not (shared.flash_erased):
        time.sleep(0.25)
        sys.stdout.write('.')
        if (time.time() - eraseStartTime) > 8:
            print"\nFlash erase timeout, retrying;"
            xb_send(shared.xb, RobName,0, command.ERASE_SECTORS, pack('L',numSamples))
            eraseStartTime = time.time()
    print "\nFlash erase done."
    
def startTelemetrySave(numSamples,RobName):
    shared.numSamples = numSamples
    print "started save"
    xb_send(shared.xb, RobName, \
            0, command.SPECIAL_TELEMETRY, pack('L',numSamples))
    
def sendMoveQueue(moveq,RobName):
    shared.moveq = moveq
    nummoves = moveq[0]
    xb_send(shared.xb, RobName, \
            0, command.SET_MOVE_QUEUE, pack('=h'+nummoves*'hhLhhhh', *moveq))
    
def setMotorSpeeds(spleft, spright):
    thrust = [spleft, 0, spright, 0, 0]
    xb_send(shared.xb, shared.DEST_ADDR, \
            0, command.SET_THRUST_CLOSED_LOOP, pack('5h',*thrust))
    

        
        
def calcNumSamples(moveq):
    #Calculates the total movement time from the move queue above
    shared.runtime = sum([moveq[i] for i in [ind*7+3 for ind in range(0,moveq[0])]])
   
    #calculate the number of telemetry packets we expect
    n = int(ceil(150 * (shared.runtime + shared.leadinTime + shared.leadoutTime) / 1000.0))
    #allocate an array to write the downloaded telemetry data into
    shared.imudata = [ [] ] * n
    print "Samples: ",n
    return n

def queryRobot(DEST_ADDR):
    #Confusing unpack is used here to turn byte string into integer
    print "Querying robot 0x%02X" % unpack('>h',DEST_ADDR)[0]
    xb_send(shared.xb, DEST_ADDR, 0,  command.WHO_AM_I, "Robot Echo")
    time.sleep(0.3)