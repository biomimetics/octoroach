import msvcrt, sys, time
from lib import command 
import shared

from or_helpers import *


###### Operation Flags ####
RESET_ROBOT = True   #Note: This MUST be False if you're using an XBee
                      # This is a known bug.


def menu():
    print "-------------------------------------"
    print "Keyboard control with Mixing,  May 6, 2011"
    print " m:menu    q:quit   w:left+    s:left-   x:left off"
    print "e:right+   d:right-  c: right off  space: all off"

def main():

    setupSerial()

    if RESET_ROBOT:
        print "Resetting robot..."
        resetRobot()
        time.sleep(1)

    menu()


    #motorgains = [200,2,0,2,0,    200,2,0,2,0]
    motorgains = [8000,100,0,0,10 , 8000,100,0,0,10] #Hardware PID
    
    setMotorGains(motorgains)
    
    tinc = 20;

    forward = 0
    turn = 0
    
    left_throt = 0
    right_throt = 0

    print "Go!"
    # Print blank space, we'll use stdout.write to display throttle vals here
    #sys.stdout.write(" "*60 + "\r")
    #sys.stdout.flush()
    #blank out any keypresses leading in...
    while msvcrt.kbhit():
        ch = msvcrt.getch()
    while True:
        keypress = msvcrt.getch()
        if keypress == ' ':
            forward = 0
            turn = 0
        elif ord(keypress) == 72: #up
            forward = forward + tinc
        elif ord(keypress) == 80: #down
            forward = forward - tinc
        elif ord(keypress) == 75: #left
            turn = turn + tinc
        elif ord(keypress) == 77: #right
            turn = turn - tinc
        elif keypress == 's':  #go straight
                forward = min(left_throt, right_throt)
                turn = 0
        elif (keypress == 'q') or (ord(keypress) == 26):
                print "\nExiting..."
                shared.xb.halt()
                shared.ser.close()
                sys.exit(0)

        if forward < 0:
            forward = 0
        
        # Mixing for turning
        left_throt = forward - turn 
        right_throt = forward + turn
        
        # Clipping to 0
        if left_throt < 0:
            left_throt = 0
        if right_throt < 0:
            right_throt = 0
        
        #Display output values
        outstring = "L: {0:03.1f}  |   R: {1:03.1f}           \r".format(left_throt,right_throt)
        sys.stdout.write(outstring)
        sys.stdout.flush()
        
        setMotorSpeeds(left_throt, right_throt)
        ### FOR REVERSED MOTORS:
        #setMotorSpeeds(right_throt, left_throt)

        time.sleep(0.05) #efective debounce, or rate limiting




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
