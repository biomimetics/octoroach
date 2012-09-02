from lib import command
import time,sys
import serial
import shared
import msvcrt

from or_helpers import *


###### Operation Flags ####
RESET_ROBOT = True
##########################

def main():
    xb = setupSerial(shared.BS_COMPORT, shared.BS_BAUDRATE)
    shared.xb = xb
    
    R1 = Robot('\x20\x52', xb)
    shared.ROBOTS = [R1]
    
    if RESET_ROBOT:
        print "Resetting robot..."
        R1.reset()
        time.sleep(0.5)  
    

    motorgains = [20000,100,0,0,0,    20000,100,0,0,0]
    R1.setMotorGains(motorgains, retries = 8)
    
    verifyAllMotorGainsSet()  #exits on failure
    
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
            xb_safe_exit()

        throttle = [0 if t<0 else t for t in throttle]
        R1.setMotorSpeeds(throttle[0],throttle[1])
        
        print "Throttle = ",throttle
        time.sleep(0.1)

def menu():
    print "-------------------------------------"
    print "Keyboard control Sep. 23, 2011"
    print " m:menu    q:quit   w:left+    s:left-   x:left off"
    print "e:right+   d:right-  c: right off  space: all off"

    

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