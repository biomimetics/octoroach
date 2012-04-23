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

ser = serial.Serial(shared.BS_COMPORT, 230400,timeout=3, rtscts=1)
xb = XBee(ser, callback = xbee_received)

def xb_send(status, type, data):
    payload = chr(status) + chr(type) + ''.join(data)
    xb.tx(dest_addr = DEST_ADDR, data = payload)

def resetRobot():
    xb_send(0, command.SOFTWARE_RESET, pack('h',0))


def menu():
    print "-------------------------------------"
    print "Keyboard control May 6, 2011"
    print " m:menu    q:quit   w:left+    s:left-   x:left off"
    print "e:right+   d:right-  c: right off  space: all off"

if __name__ == '__main__':

    dataFileName = 'imudata.txt'

    #resetRobot()
    #time.sleep(1)

    if ser.isOpen():
        print "Serial open."
    print "Keyboard Control w/ Steering Mixing - October 2011"
        
    #resetRobot()
    #time.sleep(1)
    
    STEER = 0

    motorgains = [200,2,0,2,0,    200,2,0,2,0]
    while not(shared.motor_gains_set):
        print "Setting motor gains..."
        xb_send(0, command.SET_PID_GAINS, pack('10h',*motorgains))
        time.sleep(1)
    
    throttle = [0,0]
    tinc = 20;

    forward = 0
    turn = 0

    print "Go!"
    #blank out any keypresses leading in...
    while msvcrt.kbhit():
        ch = msvcrt.getch()
    while True:
        keypress = msvcrt.getch()
        if keypress == ' ':
            throttle = [0,0]
	    forward = 0
	    turn = 0
        elif keypress == 'w':
            throttle[1] += tinc
	elif ord(keypress) == 72: #up
		forward = forward + tinc
	elif ord(keypress) == 80: #down
		forward = forward - tinc
	elif ord(keypress) == 75: #left
		turn = turn - tinc
	elif ord(keypress) == 77: #right
		turn = turn + tinc
	elif keypress == 's':  #go straight
            forward = min(left_throt, right_throt)
            turn = 0
        elif (keypress == 'q') or (ord(keypress) == 26):
            print "Exit."
            xb.halt()
            ser.close()
            sys.exit(0)

        if forward < 0:
		forward = 0
        left_throt = forward + turn 
	right_throt = forward - turn
    
	if left_throt < 0:
		left_throt = 0
	if right_throt < 0:
		right_throt = 0
    
	#print "L: ",left_throt,"  |   R: ",right_throt
	sys.stdout.write(" "*60 + "\r")
	sys.stdout.flush()
	outstring = "L: {0:03.1f}  |   R: {1:03.1f}  ".format(left_throt,right_throt)
	#outstring = "L: {0:03.3f}  |   R: {1:03.3f} \r".format(turnInput,thrustInput)
	sys.stdout.write(outstring)
	sys.stdout.flush()
	#throttle = [0 if t<0 else t for t in throttle]
	thrust = [left_throt, 0, right_throt, 0, 0]
	xb_send(0, command.SET_THRUST_CLOSED_LOOP, pack('5h',*thrust))
	#xb_send(0,command.SET_THRUST_OPEN_LOOP,pack('2h',*throttle))


