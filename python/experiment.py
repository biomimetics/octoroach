#!/usr/bin/env python
"""
authors: stanbaek, apullin

"""
from lib import command
import time,sys
import serial
import shared

from or_helpers import *
from hall_helpers import queryRobot



###### Operation Flags ####
SAVE_DATA = True
RESET_ROBOT = True   #Note: This MUST be False if you're using an XBee
                      # This is a known bug.

def main():    
    setupSerial()

    if SAVE_DATA:
        shared.dataFileName = findFileName();
        print "Data file:  ", shared.dataFileName

    #sendEcho("echo test")
    #time.sleep(0.2)

    if RESET_ROBOT:
        print "Resetting robot..."
        resetRobot()
        time.sleep(0.5)
        
    # Send robot a WHO_AM_I command, verify communications
    queryRobot()
        
    #wakeRobot()    
    #time.sleep(1)
    #sleepRobot()
    
    setSteeringRate(0)

    #Motor gains format:
    #  [ Kp , Ki , Kd , Kaw , Kff     ,  Kp , Ki , Kd , Kaw , Kff ]
    #    ----------LEFT----------        ---------_RIGHT----------
    
    motorgains = [8000,100,2,0,0 , 8000,100,2,0,0] #Hardware PID
    #motorgains = [200,2,0,2,0,    200,2,0,2,0]       #Software PID
    setMotorGains(motorgains)

    #Steering gains format:
    #  [ Kp , Ki , Kd , Kaw , Kff]
    #
    steeringGains = [5000,0,0,0,0,  STEER_MODE_DECREASE] # Disables steering controller
    #steeringGains = [20,1,0,1,0,  STEER_MODE_DECREASE]
    #steeringGains = [50,10,0,0,0,  STEER_MODE_DECREASE] # Hardware PID
    setSteeringGains(steeringGains)

    #### Do not send more than 5 move segments per packet!   ####
    #### Instead, send multiple packets, and don't use       ####
    ####    calcNumSamples() below, manually calc numSamples ####
    #### This will be fixed to be automatic in the future.   ####

    #Constant example
    #moves = 1
    #moveq = [moves, \
    #         135, 135, 10000,   MOVE_SEG_CONSTANT, 0, 0, 0]
             
    #Ramp example
    moves = 3
    moveq = [moves, \
        0,   0,   500,   MOVE_SEG_RAMP,    300, 300, 0,
        150, 150, 1000,   MOVE_SEG_CONSTANT, 0,  0,  0,
        150, 150, 500,   MOVE_SEG_RAMP, -300,  -300,  0]

    #Sin example
    #RAD_TO_BAMS16 = (0x7FFF)/(3.1415)
    #phase = 3.1415/2 * RAD_TO_BAMS16
    #moves = 2
    #moveq = [moves, \
    #         76,   76,   2000,   MOVE_SEG_SIN,  75, 1000, phase,
    #	     75, 75, 2000,   MOVE_SEG_CONSTANT, 0,  0,  0]
    
    
    #Timing settings
    shared.leadinTime = 500;
    shared.leadoutTime = 500;
    
    numSamples = calcNumSamples(moveq)
    shared.imudata = [ [] ] * numSamples
    
    #Flash must be erased to save new data
    if SAVE_DATA:
        eraseFlashMem(numSamples)

    # Pause and wait to start run, including leadin time
    raw_input("Press enter to start run ...")
    
    
    # Trigger telemetry save, which starts as soon as it is received
    if SAVE_DATA:
        startTelemetrySave(numSamples)

    time.sleep(shared.leadinTime / 1000.0)
    #Send the move queue to the robot; robot will start processing it
    #as soon as it is received
    sendMoveQueue(moveq)
    

    if SAVE_DATA:
        downloadTelemetry(numSamples)

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
