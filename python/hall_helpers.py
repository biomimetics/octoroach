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


class hallParams:
    motorgains = []
    throttle = []
    duration = []
    delta = []
    intervals = []
    vel = []
    def __init__(self, motorgains, throttle, duration, delta, intervals, vel):
        self.motorgains = motorgains
        self.throttle = throttle
        self.duration = duration
        self.delta = delta
        self.intervals = intervals
        self.vel = vel


def xb_safe_exit():
    print "Halting xb"
    shared.xb.halt()
    print "Closing serial"
    shared.ser.close()
    print "Exiting..."
    sys.exit(1)

def xb_send(status, type, data):
    payload = chr(status) + chr(type) + ''.join(data)
    shared.xb.tx(dest_addr = shared.DEST_ADDR, data = payload)

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
def getVelProfile(params):

    sum = 0
    print 'set points in degrees e.g. 60,90,180,360:',
    x = raw_input()
    if len(x):
        temp = map(int,x.split(','))
        params.delta[0] = (temp[0]*42)/360
        sum = params.delta[0]
        for i in range(1,3):
            params.delta[i] = ((temp[i]-temp[i-1])*42)/360
            sum = sum + params.delta[i]
        params.delta[3]=42-sum
    else:
        print 'not enough delta values'
        
    print 'current duration (ms)',params.duration,' new value:',
    params.duration = int(raw_input())
    print 'enter % time of each segment <csv>',
    x = raw_input()
    if len(x):
        params.intervals = map(int,x.split(','))
        sum = 0
        for i in range(0,4):
            params.intervals[i] = params.duration*params.intervals[i]/100  # interval in ms
            sum = sum + params.intervals[i]
            params.vel[i] = (params.delta[i] <<8)/params.intervals[i]
        #adjust to total duration for rounding
        params.intervals[3] = params.intervals[3] + params.duration - sum
    else:
        print 'not enough values'
 #  print 'intervals (ms)',intervals
    params.duration = params.duration -1 # end on current segment
    
    #assign locally calculated values to parameter object:
    #params.delta = delta
    #params.duration = duration
    #params.intervals = intervals
    #params.vel = vel
        

#set velocity profile
def setVelProfile(params):
    print "Sending velocity profile"
    print "set points [encoder values]", params.delta
    print "intervals (ms)",params.intervals
    print "velocities (<<8)",params.vel
    temp = 2*(params.intervals + params.delta + params.vel)
    xb_send(0, command.SET_VEL_PROFILE, pack('24h',*temp))
    time.sleep(0.3)
    

# set robot control gains
def setHallGains(motorgains):
    count = 0
    while not(shared.motor_gains_set):
        print "Setting motor gains. Packet:",count
        count = count + 1
        xb_send(0, command.SET_HALL_GAINS, pack('10h',*motorgains))
        time.sleep(0.3)
        if count > 8:
            xb_safe_exit()

# allow user to set robot gain parameters
def readinGains(lr, params):
    print 'Rmotor gains [Kp Ki Kd Kanti-wind ff]=', params.motorgains[0:5]
    print 'Lmotor gains [Kp Ki Kd Kanti-wind ff]=', params.motorgains[5:11]  
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
                params.motorgains[0:5] = motor
# temp - set both gains same
                params.motorgains[5:11] = motor
            else:
                params.motorgains[5:11] = motor
        else:
            print 'not enough gain values'
            
    

# execute move command
def proceed(params):
    thrust = [params.throttle[0], params.duration, params.throttle[1], params.duration, 0]
    xb_send(0, command.SET_THRUST_CLOSED_LOOP, pack('5h',*thrust))
    print "Throttle = ",params.throttle,"duration =", params.duration
    time.sleep(0.1)

def setupSerial():
    print "Setting up serial ..."
    try:
        shared.ser = serial.Serial(shared.BS_COMPORT, shared.BS_BAUDRATE, \
                    timeout=3, rtscts=0)
    except serial.serialutil.SerialException:
        print "Could not open serial port:",shared.BS_COMPORT
        print "Exiting ..."
        sys.exit(1)
        
    if shared.ser.isOpen():
        print "Serial open. Using port",shared.BS_COMPORT
    shared.xb = XBee(shared.ser, callback = xbee_received)


def getDstAddrString():
    return hex(256* ord(shared.DEST_ADDR[0])+ ord(shared.DEST_ADDR[1]))
    
def sendWhoAmI():
    xb_send(0, command.WHO_AM_I, "Robot Echo") 