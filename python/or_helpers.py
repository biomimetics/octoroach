import glob
import time
import sys
from lib import command
from callbackFunc import xbee_received
import datetime
import serial
import shared
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
STEER_MODE_YAW_DEC = 4
STEER_MODE_YAW_SPLIT = 5



class Robot:
    motor_gains_set = False
    steering_gains_set = False
    steering_rate_set = False
    tail_gains_set = False
    robot_queried = False
    flash_erased = False
    robot_awake = True
    motorGains = [0,0,0,0,0, 0,0,0,0,0]
    steeringGains = [0,0,0,0,0]
    tailGains = [0,0,0,0,0]
    angRateDeg = 0;
    angRate = 0;
    dataFileName = ''
    imudata = [ [] ]
    numSamples = 0
    VERBOSE = True
    
    def __init__(self, address, xb):
        self.DEST_ADDR = address
        self.DEST_ADDR_int = unpack('>h',self.DEST_ADDR)[0] #address as integer
        self.xb = xb
        print "Robot with DEST_ADDR = 0x%02X " % self.DEST_ADDR_int
    
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
    
    def wake(self, period = 0.2, retries = 8):
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

    def setMotorGains(self, gains, retries = 8):
        tries = 1
        self.motorGains = gains
        while not(self.motor_gains_set) and (tries <= retries):
            self.clAnnounce()
            print "Setting motor gains...   ",tries,"/8"
            self.tx( 0, command.SET_PID_GAINS, pack('10h',*gains))
            tries = tries + 1
            time.sleep(0.3)
        
    def setSteeringGains(self, gains, retries = 8):
        tries = 1
        self.steeringGains = gains
        while not (self.steering_gains_set) and (tries <= retries):
            self.clAnnounce()
            print "Setting steering gains...   ",tries,"/8"
            self.tx( 0, command.SET_STEERING_GAINS, pack('6h',*gains))
            tries = tries + 1
            time.sleep(0.3)
            
    def setTailGains(self, gains, retries = 8):
        tries = 1
        self.tailGains = gains
        while not (self.tail_gains_set) and (tries <= retries):
            self.clAnnounce()
            print "Setting TAIL gains...   ",tries,"/8"
            self.tx( 0, command.SET_TAIL_GAINS, pack('5h',*gains))
            tries = tries + 1
            time.sleep(0.3)

    def eraseFlashMem(self, timeout = 8):
        eraseStartTime = time.time()
        self.tx( 0, command.ERASE_SECTORS, pack('L',self.numSamples))
        self.clAnnounce()
        print "Started flash erase ..."
        while not (self.flash_erased):
            #sys.stdout.write('.')
            time.sleep(0.25)
            if (time.time() - eraseStartTime) > timeout:
                print"Flash erase timeout, retrying;"
                self.tx( 0, command.ERASE_SECTORS, pack('L',self.numSamples))
                eraseStartTime = time.time()
        
    def startTelemetrySave(self):
        self.clAnnounce()
        print "Started telemtry save"
        self.tx( 0, command.SPECIAL_TELEMETRY, pack('L',self.numSamples))
        
    def sendMoveQueue(self, moveq):
        SEG_LENGTH = 9  #might be changed in the future
        
        n = moveq[0]
        if len(moveq[1:]) != n * SEG_LENGTH:
            print "CRITICAL: Move queue length specification invalid."
            print "Wrong number of entries, len(moveq) = ",len(moveque)
            xb_safe_exit()
            
        self.nummoves = n
        self.moveq = moveq
        
        self.clAnnounce()
        print "Sending move queue with",self.nummoves," segments"
        
        segments = moveq[1:]
        #Convert to a list of lists, each sublist is one entry
        segments = [segments[i:i+SEG_LENGTH] for i in range(0,len(segments),SEG_LENGTH)]
        toSend = segments[0:4]
        
        pktCount = 1
        
        while toSend != []:
            self.clAnnounce()
            print "Move queue packet",pktCount
            numToSend = len(toSend)         #Could be < 4, since toSend still a list of lists
            toSend = [item for sublist in toSend for item in sublist]  #flatted toSend
            packet = [numToSend]
            packet.extend(toSend)    #Full moveq format to be given to pack()
            #Actual TX
            self.tx( 0, command.SET_MOVE_QUEUE, pack('=h'+numToSend*'hhLhhhhhh', *packet))
            time.sleep(0.01)                #simple holdoff, probably not neccesary
            segments = segments[4:]         #remanining unsent ones
            toSend = segments[0:4]          #Due to python indexing, this could be from 1-4
            pktCount = pktCount + 1
    
    def sendTailQueue(self, moveq):
        SEG_LENGTH = 6  #might be changed in the future
        
        n = moveq[0]
        if len(moveq[1:]) != n * SEG_LENGTH:
            print "CRITICAL: Tail queue length specification invalid."
            print "Wrong number of entries."
            xb_safe_exit()
            
        self.nummoves = n
        self.moveq = moveq
        
        self.clAnnounce()
        print "Sending TAIL queue with",self.nummoves," segments"
        
        segments = moveq[1:]
        #Convert to a list of lists, each sublist is one entry
        segments = [segments[i:i+SEG_LENGTH] for i in range(0,len(segments),SEG_LENGTH)]
        toSend = segments[0:4]
        
        pktCount = 1
        
        while toSend != []:
            self.clAnnounce()
            print "TAIL queue packet",pktCount
            numToSend = len(toSend)         #Could be < 4, since toSend still a list of lists
            toSend = [item for sublist in toSend for item in sublist]  #flatted toSend
            packet = [numToSend]
            packet.extend(toSend)    #Full moveq format to be given to pack()
            #Actual TX
            self.tx( 0, command.SET_TAIL_QUEUE, pack('=h'+numToSend*'hLhhhh', *packet))
            time.sleep(0.01)                #simple holdoff, probably not neccesary
            segments = segments[4:]         #remanining unsent ones
            toSend = segments[0:4]          #Due to python indexing, this could be from 1-4
            pktCount = pktCount + 1
        
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
    
    def downloadTelemetry(self, timeout = 6):
        #supress callback output messages for the duration of download
        self.VERBOSE = False
        self.clAnnounce()
        print "Started telemetry download"
        self.tx( 0, command.FLASH_READBACK, pack('=L',self.numSamples))
                
        dlStart = time.time()
        shared.last_packet_time = dlStart
        #bytesIn = 0
        while self.imudata.count([]) > 0:
            time.sleep(0.1)
            dlProgress(self.numSamples - self.imudata.count([]) , self.numSamples)
            if (time.time() - shared.last_packet_time) > timeout:
                print ""
                self.clAnnounce()
                print "Readback timeout exceeded, restarting."
                raw_input("Press Enter to start readback ...")
                self.imudata = [ [] ] * self.numSamples
                self.clAnnounce()
                print "Started telemetry download"
                dlStart = time.time()
                shared.last_packet_time = dlStart
                self.tx( 0, command.FLASH_READBACK, pack('=L',self.numSamples))

        dlEnd = time.time()
        #Final update to download progress bar to make it show 100%
        dlProgress(self.numSamples-self.imudata.count([]) , self.numSamples)

        #enable callback output messages
        self.VERBOSE = True

        print ""
        self.saveImudata()
        #Done with flash download and save
    
    
    def saveImudata(self):
        self.findFileName()
        self.writeFileHeader()
        fileout = open(self.dataFileName, 'a')
        np.savetxt(fileout , np.array(self.imudata), '%d,'*14+'%f,%d,%d,%f,%f,%d,%d,%d', delimiter = ',')
        fileout.close()
        self.clAnnounce()
        print "Telemtry data saved to", self.dataFileName
    
    def writeFileHeader(self):
        now = datetime.datetime.now()

        fileout = open(self.dataFileName,'w')
        #write out parameters
        fileout.write('% ' + now.strftime("%m/%d/%Y %H:%M") + '\n')
        fileout.write('%' + '  Robot: 0x%02X \n' % self.DEST_ADDR_int)
        fileout.write('%  angrate (deg) = ' + str(self.angRateDeg) + '\n')
        fileout.write('%  angrate (raw) = ' + str(self.angRate) + '\n')
        fileout.write('%  motorgains    = ' + repr(self.motorGains) + '\n')
        fileout.write('%  steeringGains = ' + repr(self.steeringGains) + '\n')
        fileout.write('%  runtime       = ' + repr(self.runtime) + '\n')
        fileout.write('%  numSamples    = ' + repr(self.numSamples) + '\n')
        fileout.write('%  moveq         = ' + repr(self.moveq) + '\n')
        fileout.write('% Columns: \n')
        fileout.write('% time | Llegs | Rlegs | DCL | DCR | GyroX | GyroY | GyroZ | GryoZAvg | AccelX | AccelY |AccelZ | LBEMF | RBEMF | SteerOut | Vbatt | SteerAngle\n')
        fileout.close()

    def setupImudata(self, moveq):
        MOVE_QUEUE_ENTRY_LEN = 9
        #Calculates the total movement time from the move queue above
        #done by striding over moveq array and summing times
        self.runtime = sum([moveq[i] for i in [(ind*MOVE_QUEUE_ENTRY_LEN)+3 for ind in range(0,moveq[0])]])
       
        #calculate the number of telemetry packets we expect
        self.numSamples = int(ceil(150 * (self.runtime + self.leadinTime + self.leadoutTime) / 1000.0))
        #allocate an array to write the downloaded telemetry data into
        self.imudata = [ [] ] * self.numSamples
        self.clAnnounce()
        print "Telemtry samples to save: ",self.numSamples

    def findFileName(self):
        filenames = glob.glob("*imudata*.txt");
        # Explicitly remove "imudata.txt", since that can mess up the pattern
        if 'imudata.txt' in filenames:
            filenames.remove('imudata.txt')

        if filenames == []:
            self.dataFileName = "imudata1.txt"
        else:
            filenames.sort()
            filenum = [int(fn[7:-4]) for fn in filenames]
            filenum.sort()
            filenum = filenum[-1] + 1
            self.dataFileName = "imudata" + str(filenum) + ".txt"
            


########## Helper functions #################


def setupSerial(COMPORT , BAUDRATE , timeout = 3, rtscts = 0):
    print "Setting up serial ..."
    try:
        ser = serial.Serial(port = COMPORT, baudrate = BAUDRATE, \
                    timeout=timeout, rtscts=rtscts)
    except serial.serialutil.SerialException:
        print "Could not open serial port:",shared.BS_COMPORT
        sys.exit()
    
    shared.ser = ser
    return XBee(ser, callback = xbee_received)
    
    

    
def xb_safe_exit():
    print "Halting xb"
    shared.xb.halt()
    print "Closing serial"
    shared.ser.close()
    print "Exiting..."
    sys.exit(1)


def dlProgress(current, total):
    percent = int(100.0*current/total)
    dashes = int(floor(percent/100.0 * 45))
    stars = 45 - dashes - 1
    barstring = '|' + '-'*dashes + '>' + '*'*stars + '|'
    #sys.stdout.write("\r" + "Downloading ...%d%%   " % percent)
    sys.stdout.write("\r" + str(current).rjust(5) +"/"+ str(total).ljust(5) + "   ")
    sys.stdout.write(barstring)
    sys.stdout.flush()
    
    
def verifyAllMotorGainsSet():
    #Verify all robots have motor gains set
    for r in shared.ROBOTS:
        if not(r.motor_gains_set):
            print "CRITICAL : Could not SET MOTOR GAINS on robot 0x%02X" % r.DEST_ADDR_int
            xb_safe_exit()

def verifyAllSteeringGainsSet():
    #Verify all robots have motor gains set
    for r in shared.ROBOTS:
        if not(r.steering_gains_set):
            print "CRITICAL : Could not SET STEERING GAINS on robot 0x%02X" % r.DEST_ADDR_int
            xb_safe_exit()
            
def verifyAllSteeringRateSet():
    #Verify all robots have motor gains set
    for r in shared.ROBOTS:
        if not(r.steering_gains_set):
            print "CRITICAL : Could not SET STEERING GAINS on robot 0x%02X" % r.DEST_ADDR_int
            xb_safe_exit()
            
def verifyAllTailGainsSet():
    #Verify all robots have motor gains set
    for r in shared.ROBOTS:
        if not(r.tail_gains_set):
            print "CRITICAL : Could not SET TAIL GAINS on robot 0x%02X" % r.DEST_ADDR_int
            xb_safe_exit()
            
def verifyAllQueried():            
    for r in shared.ROBOTS:
        if not(r.robot_queried):
            print "CRITICAL : Could not query robot 0x%02X" % r.DEST_ADDR_int
            xb_safe_exit()
