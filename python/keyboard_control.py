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


ser = serial.Serial(shared.BS_COMPORT, shared.BS_BAUDRATE,timeout=3, rtscts=0)
xb = XBee(ser, callback = xbee_received)

def xb_send(status, type, data):
    payload = chr(status) + chr(type) + ''.join(data)
    xb.tx(dest_addr = DEST_ADDR, data = payload)

def resetRobot():
    xb_send(0, command.SOFTWARE_RESET, pack('h',0))

def menu():
    print "-------------------------------------"
    print "Keyboard control Sep. 23, 2011"
    print " m:menu    q:quit   w:left+    s:left-   x:left off"
    print "e:right+   d:right-  c: right off  space: all off"

    
def main():
    dataFileName = 'imudata.txt'
    count = 0       # keep track of packet tries
    print "using destination address 0x%x" % ord(DEST_ADDR[0]),
    print "%x" % ord(DEST_ADDR[1])
    if RESET_ROBOT:
        print "Resetting robot..."
        resetRobot()
        time.sleep(1)  

    if ser.isOpen():
        print "Serial open. Using port",shared.BS_COMPORT
    

    motorgains = [200,2,0,2,0,    200,2,0,2,0]
    while not(shared.motor_gains_set):
        print "Setting motor gains. Packet:",count
        count = count + 1
        xb_send(0, command.SET_PID_GAINS, pack('10h',*motorgains))
        time.sleep(1)
        if count > 32:
            print "Count exceeded. Exit."
            xb.halt()
            ser.close()
            sys.exit(0)
        
    throttle = [0,0]
    tinc = 25;

    #blank out any keypresses leading in...
    while msvcrt.kbhit():
        ch = msvcrt.getch()
    menu()
    while True:
        keypress = msvcrt.getch()
        if keypress == ' ':
            throttle = [0,0]
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
        elif keypress == 'm':
            menu()
        elif (keypress == 'q') or (ord(keypress) == 26):
            print "Exit."
            xb.halt()
            ser.close()
            sys.exit(0)

        throttle = [0 if t<0 else t for t in throttle]
        thrust = [throttle[0], 1000, throttle[1], 1000, 0]
        xb_send(0, command.SET_THRUST_CLOSED_LOOP, pack('5h',*thrust))
        #xb_send(0,command.SET_THRUST_OPEN_LOOP,pack('2h',*throttle))
        print "Throttle = ",throttle
        time.sleep(0.1)

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
