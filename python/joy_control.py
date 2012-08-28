from lib import command
import time,sys
import serial
import shared
import pygame

from or_helpers import *


###### Operation Flags ####
SAVE_DATA   = False
RESET_ROBOT = True

MAXTHROT = 100

BUTTON_L1 = 4
BUTTON_R1 = 5

def main():
    global MAXTHROT
    xb = setupSerial(shared.BS_COMPORT, shared.BS_BAUDRATE)
    shared.xb = xb
    
    R1 = Robot('\x20\x52', xb)
    shared.ROBOTS = [R1]

    if RESET_ROBOT:
        print "Resetting robot..."
        R1.reset()
        time.sleep(0.5)

    motorgains = [30000,100,0,0,10,    30000,100,0,0,10]
    R1.setMotorGains(motorgains, retries = 8)
    
    verifyAllMotorGainsSet()  #exits on failure
        
    # Send robot a WHO_AM_I command, verify communications
    R1.query()

    j = setupJoystick()


    
    throttle = [0,0]
    tinc = 25;
  
    while True:

        value = []
        pygame.event.pump()
        
        #max throttle increase/decrease buttons
        if j.get_button(BUTTON_L1) == 1 and MAXTHROT > 0:
                MAXTHROT = MAXTHROT - 25
        elif j.get_button(BUTTON_R1) ==1 and MAXTHROT < 900:
                MAXTHROT = MAXTHROT + 25
        
        left_throt = -j.get_axis(1)
        right_throt = -j.get_axis(3)
        if left_throt < 0.05:
            left_throt = 0
        if right_throt < 0.05:
            right_throt = 0
        left_throt = int(MAXTHROT * left_throt)
        right_throt = int(MAXTHROT * right_throt)
        
        sys.stdout.write(" "*60 + "\r")
        sys.stdout.flush()
        outstring = "L: {0:3d}  |   R: {1:3d} \r".format(left_throt,right_throt)
        sys.stdout.write(outstring)
        sys.stdout.flush()
        
        R1.setMotorSpeeds(left_throt,right_throt)
        

        time.sleep(0.25)

def setupJoystick():
    try:
        pygame.init()
        j = pygame.joystick.Joystick(0)
        j.init()
        print j.get_name()
    except Exception as args:
        print 'No joystick'
        print 'Exception: ', args
        xb_safe_exit()
        
    return j


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
        xb_safe_exit()
    except serial.serialutil.SerialException:
        xb_safe_exit()
