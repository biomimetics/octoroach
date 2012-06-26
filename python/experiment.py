#!/usr/bin/env python
"""
authors: stanbaek, apullin

"""
from lib import command
import time,sys
import serial
import shared

from or_helpers import *



###### Operation Flags ####
SAVE_DATA = False
RESET_ROBOT = True   #Note: This MUST be False if you're using an XBee
                      # This is a known bug.

def main():    
    setupSerial()

    if SAVE_DATA:
        shared.dataFileName = findFileName();
        print "Data file:  ", shared.dataFileName

    #sendEcho("abc")
    #time.sleep(1)

    if RESET_ROBOT:
        print "Resetting robot..."
        resetRobot()
        time.sleep(1)
        
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

    #Constant example
    #moves = 1
    #moveq = [moves, \
    #         135, 135, 10000,   MOVE_SEG_CONSTANT, 0, 0, 0]
             
    #Ramp example
    moves = 5
    moveq = [moves, \
        0,   0,   0,   MOVE_SEG_LOOP_DECL,    0, 0, 0,
        0,   0,   500,   MOVE_SEG_RAMP,    300, 300, 0,
        150, 150, 1000,   MOVE_SEG_CONSTANT, 0,  0,  0,
        150, 150, 500,   MOVE_SEG_RAMP, -300,  -300,  0,
        0, 0, 100,   MOVE_SEG_CONSTANT, 0,  0,  0]

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
    numSamples = 850
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
    
    #Clear loop
    time.sleep(6)
    mqclear = [1, 0,   0,   0,   MOVE_SEG_LOOP_CLEAR,    0, 0, 0]
    mqflush = [1, 0,   0,   0,   MOVE_SEG_QFLUSH,    0, 0, 0]

    if SAVE_DATA:
        print "Sending loop clear!!"
        sendMoveQueue(mqclear)
        time.sleep(0.05)
        print "Sending move queue flush!!"
        sendMoveQueue(mqflush)
        
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
