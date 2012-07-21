# Keyboard control with hall effect sensing and telemetry
# last revision 2/16/2012 by RSF
import msvcrt, sys
import numpy as np
from lib import command
from struct import *
import time
from xbee import XBee
import serial
from callbackFunc import xbee_received
import shared

DEST_ADDR = '\x20\x52'
imudata_file_name = 'imudata.txt'
statedata_file_name = 'statedata.txt'
dutycycle_file_name = 'dutycycle.txt'
motordata_file_name = 'motordata.txt'
telemetry = False
imudata = []
statedata = []
dutycycles = []
motordata = []
gainsNotSet = True;
delay = 0.025

###### Operation Flags ####
RESET_ROBOT = False
##########################

# motorgains = [200,2,0,2,0,    200,2,0,2,0]
# [Kp Ki Kd Kanti-wind ff]
# now uses back emf velocity as d term
motorgains = [500,0,5,0,50, 500,0,5,0,50]
throttle = [0,0]
duration = 1000  # length of run
cycle = 250 # ms for a leg cycle
# velocity profile
# [time intervals for setpoints]
# [position increments at set points]
# [velocity increments]   muliplied by 256
delta = [10,10,12,10]  # adds up to 42 counts- should be 42.6
intervals = [87, 87, 37, 39]  # total 250 ms
#intervals = [40,40,10,10] # total 100 ms
vel = [29,29,83,69]  # = 256*delta/interval


ser = serial.Serial(shared.BS_COMPORT, shared.BS_BAUDRATE,timeout=3, rtscts=0)
xb = XBee(ser, callback = xbee_received)

def xb_send(status, type, data):
    payload = chr(status) + chr(type) + ''.join(data)
    xb.tx(dest_addr = DEST_ADDR, data = payload)

def resetRobot():
    xb_send(0, command.SOFTWARE_RESET, pack('h',0))

def menu():
    print "-------------------------------------"
    print "e: radio echo test    | g: right motor gains | h: Help menu"
    print "l: left motor gains"
    print "m: toggle memory mode | n: get robot name    | p: proceed"
    print "q: quit               | r: reset robot       | s: set throttle"
    print "t: time of move length| v: set velocity profile"
    print "z: zero motor counts"


#get velocity profile
def getVelProfile():
    global cycle, intervals, vel
    sum = 0
    print 'set points in degrees e.g. 60,90,180,360:',
    x = raw_input()
    if len(x):
        temp = map(int,x.split(','))
        delta[0] = (temp[0]*42)/360
        sum = delta[0]
        for i in range(1,3):
            delta[i] = ((temp[i]-temp[i-1])*42)/360
            sum = sum + delta[i]
        delta[3]=42-sum
    else:
        print 'not enough delta values'
    print 'current cycle (ms)',cycle,' new value:',
    cycle = int(raw_input())
    print 'enter % time of each segment <csv>',
    x = raw_input()
    if len(x):
        intervals = map(int,x.split(','))
        sum = 0
        for i in range(0,4):
            intervals[i] = cycle*intervals[i]/100  # interval in ms
            sum = sum + intervals[i]
            vel[i] = (delta[i] <<8)/intervals[i]
        #adjust to total duration for rounding
        intervals[3] = intervals[3] + cycle - sum
    else:
        print 'not enough values'
 #  print 'intervals (ms)',intervals



#set velocity profile
def setVelProfile():
    global intervals, vel
    print "Sending velocity profile"
    print "set points [encoder values]", delta
    print "intervals (ms)",intervals
    print "velocities (<<8)",vel
    temp = intervals+delta+vel
    temp = temp+temp  # left = right
    xb_send(0, command.SET_VEL_PROFILE, pack('24h',*temp))
    time.sleep(1)




# set robot control gains
def setGain():
    count = 0
    while not(shared.motor_gains_set):
        print "Setting motor gains. Packet:",count
        count = count + 1
        xb_send(0, command.SET_PID_GAINS, pack('10h',*motorgains))
        time.sleep(2)
        if count > 32:
            print "count exceeded. Exit."
            print "Halting xb"
            xb.halt()
            print "Closing serial"
            ser.close()
            print "Exiting..."
            sys.exit(0)

# allow user to set robot gain parameters
def getGain(lr):
    print 'Rmotor gains [Kp Ki Kd Kanti-wind ff]=', motorgains[0:5]
    print 'Lmotor gains [Kp Ki Kd Kanti-wind ff]=', motorgains[5:11]
    x = None
    while not x:
        try:
            print 'Enter ',lr,'motor gains ,<csv> [Kp, Ki, Kd, Kanti-wind, ff]',
            x = raw_input()
        except ValueError:
            print 'Invalid Number'
    if len(x):
        motor = map(int,x.split(','))
        if len(motor) == 5:
            print lr,'motor gains', motor
# enable sensing gains again
            shared.motor_gains_set = False
            if lr == 'R':
                motorgains[0:5] = motor
           #     motorgains[5:11] = motor
            else:
                motorgains[5:11] = motor
        else:
            print 'not enough gain values'

# execute move command
count = 300

def proceed():
    global duration, count, delay, throttle
    thrust = [throttle[0], duration, throttle[1], duration, 0]
    if telemetry:
        xb_send(0, command.ERASE_SECTORS, pack('h',0))
        print "started erase, 3 second dwell"
        time.sleep(3)
        start = 0   # two byte start time to record
        skip = 0    # store every other sample if = 1
        temp=[count,start,skip]
        print 'temp =',temp,'\n'
        xb_send(0, command.START_TELEM, pack('3h',*temp))
        time.sleep(0.1)
    xb_send(0, command.SET_THRUST_CLOSED_LOOP, pack('5h',*thrust))
    print "Throttle = ",throttle,"duration =", duration
    time.sleep(0.1)
    if telemetry:
        flashReadback()

def flashReadback():
    global count, dataFileName
    raw_input("Press any key to start readback ...")
    print "started readback"
    shared.imudata = []  # reset imudata structure
    shared.pkts = 0  # reset packet count???
    xb_send(0, command.FLASH_READBACK, pack('=h',count))
    # While waiting, write parameters to start of file
    writeFileHeader(dataFileName)
    time.sleep(delay*count + 3)
    while shared.pkts != count:
        print "Retry"
        shared.imudata = []
        shared.pkts = 0
        xb_send(0, command.FLASH_READBACK, pack('=h',count))
        time.sleep(delay*count + 3)
        if shared.pkts > count:
            print "too many packets"
            break
    print "readback done"
    fileout = open(dataFileName, 'a')
    np.savetxt(fileout , np.array(shared.imudata), '%d', delimiter = ',')
    fileout.close()
    print "data saved to ",dataFileName



def writeFileHeader(dataFileName):
    fileout = open(dataFileName,'w')
    #write out parameters in format which can be imported to Excel
    today = time.localtime()
    date = str(today.tm_year)+'/'+str(today.tm_mon)+'/'+str(today.tm_mday)+'  '
    date = date + str(today.tm_hour) +':' + str(today.tm_min)+':'+str(today.tm_sec)
    fileout.write('"Data file recorded ' + date + '"\n')
    fileout.write('"%  keyboard_telem with hall effect "\n')
    fileout.write('"%  motorgains    = ' + repr(motorgains) + '\n')
    fileout.write('"%  delta         = ' +repr(delta) + '"\n')
    fileout.write('"%  intervals     = ' +repr(intervals) + '"\n')
    fileout.write('"% Columns: "\n')
    # order for wiring on RF Turner
    fileout.write('"% time | Rlegs | Llegs | DCR | DCL | GyroX | GryoY | GryoZ | GryoZAvg | AX | AY | AZ | RBEMF | LBEMF "\n')
 #   fileout.write('"% time | Rlegs | Llegs | DCL | DCR | GyroX | GryoY | GryoZ | GryoZAvg | AX | AY | AZ | LBEMF | RBEMF | SteerOut"\n')
  #  fileout.write('time, Rlegs, Llegs, DCL, DCR, GyroX, GryoY, GryoZ, GryoZAvg, AX, AY, AZ, LBEMF, RBEMF, SteerOut\n')
    fileout.close()

def main():
    print 'keyboard_telem Feb. 16, 2012\n'
    global throttle, duration, telemetry, dataFileName
    dataFileName = 'Data/imudata.txt'
    count = 0       # keep track of packet tries
    print "using robot address", hex(256* ord(DEST_ADDR[0])+ ord(DEST_ADDR[1]))
    if RESET_ROBOT:
        print "Resetting robot..."
        resetRobot()
        time.sleep(1)

    if ser.isOpen():
        print "Serial open. Using port",shared.BS_COMPORT

    setGain()
    xb_send(0, command.WHO_AM_I, "Robot Echo")
    time.sleep(0.5)  # wait for whoami before sending next command
    setVelProfile()
    throttle = [0,0]
    tinc = 25;
    # time in milliseconds
   # duration = 42*16-1  # 21.3 gear ratio, 2 counts/motor rev
   # duration = 5*100 -1  # integer multiple of time steps

    #blank out any keypresses leading in...
    while msvcrt.kbhit():
        ch = msvcrt.getch()
    menu()
    while True:
        print '>',
        keypress = msvcrt.getch()
        if keypress == ' ':
            throttle = [0,0]
        elif keypress == 'c':
            throttle[0] = 0
        elif keypress == 'd':
            throttle[0] -= tinc
        elif keypress == 'e':
            xb_send(0, command.ECHO,  "Echo Test")
            throttle[0] += tinc
        elif keypress == 'g':
            getGain('R')
            setGain()
        elif keypress == 'h':
            menu()
        elif keypress == 'l':
            getGain('L')
            setGain()
        elif keypress =='m':
            telemetry = not(telemetry)
            print 'Telemetry recording', telemetry
        elif keypress =='n':
            xb_send(0, command.WHO_AM_I, "Robot Echo")
        elif (keypress == 'p'):
             proceed()
        elif keypress == 'r':
            resetRobot()
            print 'Resetting robot'
        elif keypress == 's':  # set speed with throttle
            print "throttle = ", throttle, "enter throttle [0]:",
            throttle[0]= int(raw_input())
            print "enter throttle[1]:",
            throttle[1]= int(raw_input())
            print "new throttle =", throttle
        elif keypress == 't':
            print 'cycle='+str(cycle)+' duration='+str(duration)+'. New duration:',
            duration = int(raw_input())
        elif keypress =='v':
            getVelProfile()
            setVelProfile()
        elif keypress == 'w':
            throttle[1] += tinc
            print "Throttle = ",throttle
        elif keypress == 'x':
            throttle[1] = 0
        elif keypress == 'z':
            xb_send(0, command.ZERO_POS,  "Zero motor")
            print 'read motorpos and zero'
        elif (keypress == 'q') or (ord(keypress) == 26):
            print "Exit."
            xb.halt()
            ser.close()
            sys.exit(0)
        else:
            print "** unknown keyboard command** \n"
            menu()




#Provide a try-except over the whole main function
# for clean exit. The Xbee module should have better
# provisions for handling a clean exit, but it doesn't.
if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        xb.halt()
        ser.close()
    except IOError:
        print "IO Error."
        xb.halt()
        ser.close()
