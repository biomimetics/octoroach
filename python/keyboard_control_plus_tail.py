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

ser = serial.Serial(shared.BS_COMPORT, shared.BS_BAUDRATE,timeout=3, rtscts=0)
xb = XBee(ser, callback = xbee_received)

def xb_send(status, type, data):
    payload = chr(status) + chr(type) + ''.join(data)
    xb.tx(dest_addr = DEST_ADDR, data = payload)

def resetRobot():
    xb_send(0, command.SOFTWARE_RESET, pack('h',0))


if __name__ == '__main__':

    dataFileName = 'imudata.txt'

    #resetRobot()
    time.sleep(1)

    if ser.isOpen():
        print "Serial open."

    motorgains = [200,8,0,8,0,    200,8,0,8,0]
    try:
        while not(shared.motor_gains_set):
            print "Setting motor gains..."
            xb_send(0, command.SET_PID_GAINS, pack('10h',*motorgains))
            time.sleep(1)
    except KeyboardInterrupt:
        xb.halt()
        ser.close()
        print "Aborting..."
        sys.exit(0)
    
    throttle = [0,0]
    tail = 0;
    tinc = 300;
    flick = [0.0, 0]
    

    #blank out any keypresses leading in...
    while msvcrt.kbhit():
        ch = msvcrt.getch()
    
    while True:
        keypress = msvcrt.getch()
        if keypress == ' ':
            throttle = [0,0]
            tail = 0
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
        elif keypress == 'r':
            flick = [100.0, 240]
            xb_send(0,command.TAIL_FLICK, pack('fh',*flick))
            time.sleep(0.05)
        elif keypress == 'f':
            flick = [-100.0, 240]
            xb_send(0,command.TAIL_FLICK, pack('fh',*flick))
            time.sleep(0.05)
        #elif keypress == 'm':
        #    tail = 100
        #elif keypress == 'n':
        #    tail = -100
        
            
        elif (keypress == 'q') or (ord(keypress) == 26):
            print "Exit."
            xb.halt()
            ser.close()
            sys.exit(0)

        throttle = [0 if t<0 else t for t in throttle]
        thrust = [throttle[0], 1000, throttle[1], 1000, 0]
        xb_send(0, command.SET_THRUST_CLOSED_LOOP, pack('5h',*thrust))
        #time.sleep(0.1)
        #xb_send(0, command.SET_TAIL_POSITION, pack('f', tail))
        #xb_send(0,command.SET_THRUST_OPEN_LOOP,pack('2h',*throttle))
        print "Throttle = ",throttle, "  Tail = ",tail
        #time.sleep(0.1)

