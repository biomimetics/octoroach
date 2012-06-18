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

imudata = []
statedata = []
dutycycles = []
motordata = []
gainsNotSet = True;

###### Operation Flags ####
RESET_ROBOT = False
##########################

# motorgains = [200,2,0,2,0,    200,2,0,2,0]
# [Kp Ki Kd Kanti-wind ff]
# now uses back emf velocity as d term
motorgains = [400,0,400,0,200, 400,0,200,0,200]
throttle = [0,0]
duration = 42*16 - 1
# velocity profile
# [time intervals for setpoints]
# [position increments at set points]
# [velocity increments]   muliplied by 256
#intervals = [0xe0, 0x70, 0xe0, 0x70]  # total 672 ms, = 16 ms per half rev
delta = [11,10,10,11]  # adds up to 42 counts- should be 42.6
#intervals = [0x11e, 0x32, 0x32, 0x11e]  # total 672 ms, = 16 ms per half rev
#vel = [9,51,51,9]
intervals = [0x8f, 0x19, 0x19, 0x8f]  # total 336 ms
vel = [17,112,112,17]


ser = serial.Serial(shared.BS_COMPORT, shared.BS_BAUDRATE,timeout=3, rtscts=0)
xb = XBee(ser, callback = xbee_received)

def xb_send(status, type, data):
    payload = chr(status) + chr(type) + ''.join(data)
    xb.tx(dest_addr = DEST_ADDR, data = payload)

def resetRobot():
    xb_send(0, command.SOFTWARE_RESET, pack('h',0))

#send user selected command
def rawCommand():
    xb_send(0,0xff, pack('h',0))

def menu():
    print "-------------------------------------"
    print "Keyboard control Nov. 12, 2011"
    print "m:menu     |q:quit     |r:reset      |n: name?"
    print "w:left+    |s:left-    |x:left off   |b: bogus command"
    print "e:right+   |d:right-   |c:right off  |<sp>: all off"
    print "g:R gain   |l: L gain  |t:duration   |v: vel profile |p: proceed"
    
#get velocity profile
def getVelProfile():
    global duration, intervals, vel
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
    print 'current duration (ms)',duration,' new value:',
    duration = int(raw_input())
    print 'enter % time of each segment <csv>',
    x = raw_input()
    if len(x):
        intervals = map(int,x.split(','))
        sum = 0
        for i in range(0,4):
            intervals[i] = duration*intervals[i]/100  # interval in ms
            sum = sum + intervals[i]
            vel[i] = (delta[i] <<8)/intervals[i]
        #adjust to total duration for rounding
        intervals[3] = intervals[3] + duration - sum
    else:
        print 'not enough values'
 #  print 'intervals (ms)',intervals
    duration = duration -1 # end on current segment
        

#set velocity profile
def setVelProfile():
    global duration, intervals, vel
    print "Sending velocity profile"
    print "set points [encoder values]", delta
    print "intervals (ms)",intervals
    print "velocities (<<8)",vel
    temp = intervals+delta+vel
    temp = temp+temp
    xb_send(0, command.SET_VEL_PROFILE, pack('24h',*temp))
    time.sleep(1)
   
    


# set robot control gains
def setGain():
    count = 0
    while not(shared.motor_gains_set):
        print "Setting motor gains. Packet:",count
        count = count + 1
        xb_send(0, command.SET_HALL_GAINS, pack('10h',*motorgains))
        time.sleep(2)
        if count > 32:
            print "Count exceeded. Exit."
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
# temp - set both gains same
                motorgains[5:11] = motor
            else:
                motorgains[5:11] = motor
        else:
            print 'not enough gain values'
            
# execute move command
def proceed():
    global duration
    thrust = [throttle[0], duration, throttle[1], duration, 0]
    xb_send(0, command.SET_THRUST_CLOSED_LOOP, pack('5h',*thrust))
    print "Throttle = ",throttle,"duration =", duration
    time.sleep(0.1)

    
def main():
    global duration
    
    shared.robotQueried = False
    queries = 1
    while not(shared.robotQueried) and (queries < shared.maxQueries):
        print "Querying robot, try ",queries,"/",shared.maxQueries
        xb_send(0, command.WHO_AM_I, "Robot Echo")
        time.sleep(1)
        queries = queries + 1
        
    
    dataFileName = 'imudata.txt'
    count = 0       # keep track of packet tries
    print "using robot address", hex(256* ord(DEST_ADDR[0])+ ord(DEST_ADDR[1]))
    if RESET_ROBOT:
        print "Resetting robot..."
        resetRobot()
        time.sleep(1)  

    if ser.isOpen():
        print "Serial open. Using port",shared.BS_COMPORT
  
    setGain()
          
    throttle = [0,0]
    tinc = 25;
    # time in milliseconds
    duration = 42*16-1  # 21.3 gear ratio, 2 counts/rev

    #blank out any keypresses leading in...
    while msvcrt.kbhit():
        ch = msvcrt.getch()
    menu()
    while True:
        print '>',
        keypress = msvcrt.getch()
        if keypress == ' ':
            throttle = [0,0]
        elif keypress == 'b':
            rawCommand()
        elif keypress == 'w':
            throttle[1] += tinc
        elif keypress == 'e':
            throttle[0] += tinc
        elif keypress == 's':
            throttle[1] -= tinc
        elif keypress == 'd':
            throttle[0] -= tinc
        elif keypress == 'x':
            throttle[1] = 0
        elif keypress == 'c':
            throttle[0] = 0
        elif keypress == 'g':
            getGain('R')
            setGain()
        elif keypress == 'l':
            getGain('L')
            setGain    
        elif keypress =='v':
            getVelProfile()
            setVelProfile()
        elif keypress == 'm':
            menu()
        elif keypress =='n':
            xb_send(0, command.WHO_AM_I, "Robot Echo")      
        elif keypress == 'r':
            resetRobot()
            print 'Resetting robot'
        elif keypress == 't':
            print 'current duration',duration,' new value:',
            duration = int(raw_input())
        elif (keypress == 'p'):
             proceed()
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
        print "Error."
        xb.halt()
        ser.close()
