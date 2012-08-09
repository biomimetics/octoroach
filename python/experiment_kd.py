#!/usr/bin/env python
"""
authors: stanbaek, apullin

"""
from lib import command
import time,sys
import serial
import shared_kd as shared

from or_helpers_kd import *


###### Operation Flags ####
SAVE_DATA1 = False #Be sure to edit the if statments to the proper robot
SAVE_DATA2 = False
RESET_R1 = True  
RESET_R2 = True  

def main():    
    xb = setupSerial(shared.BS_COMPORT, shared.BS_BAUDRATE)
    
    R1 = Robot('\x20\x52', xb)
    R2 = Robot('\x20\x53', xb)
    shared.ROBOTS = [R1, R2]
    shared.xb = xb
    
    #if SAVE_DATA1:
    #    shared.dataFileName = findFileName();
    #    print "Data file:  ", shared.dataFileName

    if RESET_R1:
        R1.reset()
        time.sleep(0.25)
    if RESET_R2:
        R2.reset()
        time.sleep(0.25)
    
    # Query
    R1.query( retries = 3 )
    R2.query( retries = 3 )
    
    #Verify all robots can be queried
    for r in shared.ROBOTS:
        if not(r.robot_queried):
            print "CRITICAL : Could not query robot 0x%02X" % r.DEST_ADDR_int
            xb_safe_exit()
    
    # Steering controller setpoint
    R1.setSteeringRate(0 , retries = 3)
    R2.setSteeringRate(0 , retries = 3)
    
    #Verify all robots have steering gains set
    for r in shared.ROBOTS:
        if not(r.steering_rate_set):
            print "CRITICAL : Could not SET STEERING RATE on robot 0x%02X" % r.DEST_ADDR_int
            xb_safe_exit()
            
    xb_safe_exit()

    #Motor gains format:
    #  [ Kp , Ki , Kd , Kaw , Kff     ,  Kp , Ki , Kd , Kaw , Kff ]
    #    ----------LEFT----------        ---------_RIGHT----------
    
    motorgains = [8000,100,2,0,0 , 8000,100,2,0,0] #Hardware PID
    #motorgains = [200,2,0,2,0,    200,2,0,2,0]       #Software PID

    setMotorGains(motorgains,ROBOT2)
    setMotorGains(motorgains,ROBOT1)
    setMotorGains(motorgains,ROBOT3)
##    setMotorGains(motorgains,ROBOT4)

    #Steering gains format:
    #  [ Kp , Ki , Kd , Kaw , Kff]
    
    NegCsteeringGains = [-5000,0,0,0,0,  STEER_MODE_DECREASE] 
    PosCsteeringGains = [5000,0,0,0,0,  STEER_MODE_DECREASE]
    NOsteeringGains = [0,0,0,0,0,  STEER_MODE_DECREASE]# Disables steering controller
    steeringGains = [20,1,0,1,0,  STEER_MODE_DECREASE]
    HWPIDsteeringGains = [50,10,0,0,0,  STEER_MODE_DECREASE] # Hardware PID

    setSteeringGains(NegCsteeringGains,ROBOT1)
    setSteeringGains(NegCsteeringGains,ROBOT2)
    setSteeringGains(PosCsteeringGains,ROBOT3)
##    setSteeringGains(steeringGains,ROBOT4)

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
    moveq1 = [moves, \
        0,   0,   500,   MOVE_SEG_RAMP,    300, 300, 0,
        150, 150, 8000,   MOVE_SEG_CONSTANT, 0,  0,  0,
        150, 150, 500,   MOVE_SEG_RAMP, -300,  -300,  0]

##        moveq1 = [moves, \
##  NegC    right, left
##  PosC    left, right
##          0,   0,   500,   MOVE_SEG_RAMP,    300, 300, 0,
##          150, 150, 8000,   MOVE_SEG_CONSTANT, 0,  0,  0,
##          150, 150, 500,   MOVE_SEG_RAMP, -300,  -300,  0]

    moveq2 = [moves, \
        0,   0,   500,   MOVE_SEG_RAMP,    300, 300, 0,
        50, 50, 15000,   MOVE_SEG_CONSTANT, 0,  0,  0,
        50, 50, 500,   MOVE_SEG_RAMP, -300,  -300,  0]

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
    
    numSamples = calcNumSamples(moveq1)
    shared.imudata = [ [] ] * numSamples
    
    #Flash must be erased to save new data
    if SAVE_DATA1:
        eraseFlashMem(numSamples,ROBOT1)
        eraseFlashMem(numSamples,ROBOT2)
        eraseFlashMem(numSamples,ROBOT3)
##        eraseFlashMem(numSamples,ROBOT4)

    # Pause and wait to start run, including leadin time
    raw_input("Press enter to start run ...")
    
    
    # Trigger telemetry save, which starts as soon as it is received
    
    #### Make when saving anything, this if is set ####
    #### to the proper "SAVE_DATA"                 ####
    
    if SAVE_DATA1:
        startTelemetrySave(numSamples,ROBOT1)
        startTelemetrySave(numSamples,ROBOT2)
        startTelemetrySave(numSamples,ROBOT3)
##        startTelemetrySave(numSamples,ROBOT4)

    time.sleep(shared.leadinTime / 1000.0)
    #Send the move queue to the robot; robot will start processing it
    #as soon as it is received
    sendMoveQueue(moveq1,ROBOT1)
    sendMoveQueue(moveq1,ROBOT2)
    sendMoveQueue(moveq1,ROBOT3)
##    sendMoveQueue(moveq,ROBOT4)
    

    if SAVE_DATA1:
        downloadTelemetry(numSamples,ROBOT1)

    if SAVE_DATA2:
        shared.dataFileName = findFileName()
        print "Data file:  ", shared.dataFileName
        downloadTelemetry(numSamples,ROBOT2)

    if SAVE_DATA3:
        shared.dataFileName = findFileName();
        print "Data file:  ", shared.dataFileName
        downloadTelemetry(numSamples,ROBOT3)

    if SAVE_DATA4:
        shared.dataFileName = findFileName();
        print "Data file:  ", shared.dataFileName
        downloadTelemetry(numSamples,ROBOT4)

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
#        shared.xb.halt()
#        shared.ser.close()
#    except Exception as args:
#        print "\nGeneral exception:",args
#        print "Attemping to exit cleanly..."
        shared.xb.halt()
        shared.ser.close()
        sys.exit()