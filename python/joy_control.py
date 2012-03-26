import numpy as np
from lib import command 
from struct import *
import time
from xbee import XBee
import serial
import glob
from math import ceil
from callbackFunc import xbee_received
import shared
import pygame
import sys

DEST_ADDR = '\x20\x52'
#~ imudata_file_name = 'imudata.txt'
#~ statedata_file_name = 'statedata.txt'
#~ dutycycle_file_name = 'dutycycle.txt'
#~ motordata_file_name = 'motordata.txt'

imudata = []
statedata = []
dutycycles = []
motordata = []
gainsNotSet = True;

MAXTHROT = 250

ser = serial.Serial(shared.BS_COMPORT, shared.BS_BAUDRATE,timeout=3, rtscts=1)
xb = XBee(ser, callback = xbee_received)

def xb_send(status, type, data):
    payload = chr(status) + chr(type) + ''.join(data)
    xb.tx(dest_addr = DEST_ADDR, data = payload)

def resetRobot():
    xb_send(0, command.SOFTWARE_RESET, pack('h',0))


if __name__ == '__main__':

    #dataFileName = 'imudata.txt'

    if ser.isOpen():
        print "Serial open."
        
    #resetRobot()
    time.sleep(0.2)

    try:
        pygame.init()
        j = pygame.joystick.Joystick(0)
        j.init()
        print j.get_name()
    except:
        print 'No joystick'
        xb.halt()
        ser.close()
        sys.exit(-1)

    
    motorgains = [200,2,0,2,0,    200,2,0,2,0]
    
    while not(shared.motor_gains_set):
        print "Setting motor gains..."
        xb_send(0, command.SET_PID_GAINS, pack('10h',*motorgains))
        time.sleep(1)
    
    throttle = [0,0]

    try:    
        while True:

            value = []
            pygame.event.pump()
            left_throt = -j.get_axis(2)
            right_throt = -j.get_axis(1)
            #Clip at less than 2% throttle
            if left_throt < 0.02:
                left_throt = 0
            if right_throt < 0.02:
                right_throt = 0
            left_throt = MAXTHROT * left_throt
            right_throt = MAXTHROT * right_throt
            #print "L: ",left_throt,"  |   R: ",right_throt
            sys.stdout.write(" "*60 + "\r")
            sys.stdout.flush()
            outstring = "L: {0:03.1f}  |   R: {1:03.1f} \r".format(left_throt,right_throt)
            sys.stdout.write(outstring)
            sys.stdout.flush()
            #throttle = [0 if t<0 else t for t in throttle]
            thrust = [left_throt, 0, right_throt, 0, 0]
            xb_send(0, command.SET_THRUST_CLOSED_LOOP, pack('5h',*thrust))
            #xb_send(0,command.SET_THRUST_OPEN_LOOP,pack('2h',*throttle))

            time.sleep(0.15)

    except:
        print "closing"
        try:
            xb.halt()
            ser.close()
        except serial.SerialException:
            print "Got SerialException."









