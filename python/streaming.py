#!/usr/bin/env python
"""
authors: apullin

"""
from lib import command
import time,sys
import serial
import shared

from or_helpers import *
from hall_helpers import queryRobot



###### Operation Flags ####
#SAVE_DATA = False
RESET_ROBOT = True   #Note: This MUST be False if you're using an XBee
                      # This is a known bug.

def main():    
    setupSerial()

    #if SAVE_DATA:
    #    shared.dataFileName = findFileName();
    #    print "Data file:  ", shared.dataFileName

    if RESET_ROBOT:
        print "Resetting robot..."
        resetRobot()
        time.sleep(0.5)
        
    # Send robot a WHO_AM_I command, verify communications
    queryRobot()
        
    
    #Flash must be erased to save new data
    #if SAVE_DATA:
    #    eraseFlashMem(numSamples)

    # Pause and wait to start run, including leadin time
    raw_input("Press enter to start streaming ...")
    
    
    numSamples = 100
    startTelemetryStream(numSamples)

    #Wait for streaming to complete
    time.sleep(numSamples / 20.0 + 0.5)

    #Wait for Ctrl+C to exit; this is done in case other messages come in
    #from the robot, which are handled by callbackfunc
    print "Ctrl + C to exit"

    while True:
        try:
            time.sleep(1)
            #print ".",
        except KeyboardInterrupt:
            break

    shared.xb.halt()
    shared.ser.close()


    print "Done"

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
