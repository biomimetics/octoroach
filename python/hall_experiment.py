import msvcrt, sys
import numpy as np
from lib import command 
from struct import *
import time
from xbee import XBee
import serial
from callbackFunc import xbee_received
import shared

from or_helpers import *
from hall_helpers import *

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
RESET_ROBOT = True
##########################

# motorgains = [200,2,0,2,0,    200,2,0,2,0]
# [Kp Ki Kd Kanti-wind ff]
# now uses back emf velocity as d term
motorgains = [400,0,400,0,200, 400,0,200,0,200]
throttle = [0,0]
duration = 42*16 - 1 # 21.3 gear ratio, 2 counts/rev
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



def main():
    global motorgains, throttle, duration, delta, intervals, vel
    
    setupSerial()
    
    params = hallParams(motorgains, throttle, duration, delta, intervals, vel)
    
    # Send robot a WHO_AM_I command, verify communications
    queryRobot()
        
    
    dataFileName = 'imudata.txt'
    count = 0       # keep track of packet tries
    print "using robot address", getDstAddrString()

        
    if RESET_ROBOT:
        print "Resetting robot..."
        resetRobot()
        time.sleep(1) 
        
  
    setHallGains(motorgains)
          
    tinc = 25;

    #blank out any keypresses leading in...
    while msvcrt.kbhit():
        ch = msvcrt.getch()
    menu()
    while True:
        print '>',
        keypress = msvcrt.getch()
        if keypress == ' ':
            params.throttle = [0,0]
            print "Throttle = ",params.throttle,"duration =", params.duration
        elif keypress == 'b':
            rawCommand() #this may be unsafe
        elif keypress == 'w':
            params.throttle[1] += tinc
            print "Throttle = ",params.throttle,"duration =", params.duration
        elif keypress == 'e':
            params.throttle[0] += tinc
            print "Throttle = ",params.throttle,"duration =", params.duration
        elif keypress == 's':
            params.throttle[1] -= tinc
            print "Throttle = ",params.throttle,"duration =", params.duration
        elif keypress == 'd':
            params.throttle[0] -= tinc
            print "Throttle = ",params.throttle,"duration =", params.duration
        elif keypress == 'x':
            params.throttle[1] = 0
            print "Throttle = ",params.throttle,"duration =", params.duration
        elif keypress == 'c':
            params.throttle[0] = 0
            print "Throttle = ",params.throttle,"duration =", params.duration
        elif keypress == 'g':
            motorgains = getGain('R',params)
            setHallGains(params.motorgains)
        elif keypress == 'l':
            motorgains = getGain('L',params)
            setHallGains(params.motorgains)    
        elif keypress =='v':
            getVelProfile(params)
            setVelProfile(params)
        elif keypress == 'm':
            menu()
        elif keypress =='n':
            getWhoAmI()
        elif keypress == 'r':
            resetRobot()
            print 'Resetting robot'
        elif keypress == 't':
            print 'current duration',duration,' new value:',
            duration = int(raw_input())
        elif (keypress == 'p'):
             proceed(params)
        elif (keypress == 'q') or (ord(keypress) == 26):
            print "Exit."
            xb_safe_exit()
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
        print "\nRecieved Ctrl+C, exiting."
        shared.xb.halt()
        shared.ser.close()
    except Exception as args:
        print "\nGeneral exception:",args
        print "Attemping to exit cleanly..."
        shared.xb.halt()
        shared.ser.close()
        sys.exit()
    except serial.serialutil.SerialException:
        shared.xb.halt()
        shared.ser.close()
