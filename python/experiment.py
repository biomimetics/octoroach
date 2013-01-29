#!/usr/bin/env python
"""
authors: apullin

Contents of this file are copyright Andrew Pullin, 2013

"""
from lib import command
import time,sys
import serial
import shared

from or_helpers import *


###### Operation Flags ####
SAVE_DATA1 = False 
RESET_R1 = True  

EXIT_WAIT   = False

def main():    
    xb = setupSerial(shared.BS_COMPORT, shared.BS_BAUDRATE)
    
    R1 = Robot('\x20\x52', xb)
    
    shared.ROBOTS = [R1] #This is neccesary so callbackfunc can reference robots
    shared.xb = xb           #This is neccesary so callbackfunc can halt before exit
    
    #if SAVE_DATA1:
    #    shared.dataFileName = findFileName();
    #    print "Data file:  ", shared.dataFileName

    if RESET_R1:
        R1.reset()
        time.sleep(0.35)
    
    # Query
    R1.query( retries = 8 )
    
    #Verify all robots can be queried
    verifyAllQueried()  #exits on failure

    
    #Motor gains format:
    #  [ Kp , Ki , Kd , Kaw , Kff     ,  Kp , Ki , Kd , Kaw , Kff ]
    #    ----------LEFT----------        ---------_RIGHT----------
    
    #motorgains = [15000,50,1000,0,0,    15000,50,1000,0,0] #Hardware PID
    motorgains = [15000,50,1000,0,0,    15000,50,1000,0,0]

    R1.setMotorGains(motorgains, retries = 8)
    #Verify all robots have motor gains set
    verifyAllMotorGainsSet()   #exits on failure

    #Steering gains format:
    #  [ Kp , Ki , Kd , Kaw , Kff]
    steeringGains = [15000,5,0,0,0,  STEER_MODE_SPLIT] # Hardware PID

    R1.setSteeringGains(steeringGains, retries = 8)
    #Verify all robots have steering gains set
    verifyAllSteeringGainsSet()  #exits on failure
    
    #tailGains = [0, 0, 0, 0, 0]
    #R1.setTailGains(tailGains)
    #Verify all robots have tail gains set
    #verifyAllTailGainsSet()  #exits on failure
    
    # Steering controller setpoint
    #R1.setSteeringRate(0 , retries = 8)
    #Verify all robots have steering rate set
    #verifyAllSteeringRateSet()  #exits on failure

    #### Do not send more than 5 move segments per packet!   ####
    #### Instead, send multiple packets, and don't use       ####
    ####    calcNumSamples() below, manually calc numSamples ####
    #### This will be fixed to be automatic in the future.   ####

    #Move segment format:
    # [value1, value2 , segtime , move_seg_type , param1 , param2, param3]
    # - value1 , value2 are initial setpoints for leg speed for each segment
    # - segtime is the length of the segment, in milliseconds
    # - move_seg_type is one of the types enumerated at the top of or_helpers.py
    # MOVE_SEG_CONSTANT: param1 , param2 , param3 have no effect, set to 0.
    # MOVE_SEG_RAMP    : param1 and param2 are left/right ramp rates, in legs speed per
    #       second. param3 has no effect.
    # MOVE_SEG_SIN     : Not implemented.
    # MOVE_SEG_TRI     : Not implemented.
    # MOVE_SEG_SAW     : Not implemented.
    # MOVE_SEG_IDLE    : value1,value2 and params have no efffect. Disables leg speed controller.
    # MOVE_SEG_LOOP_DECL: Turns on move queue looping. value1,value2, and params have no effect.
    # MOVE_SEG_LOOP_CLEAR: Turns off move queue looping.value1,value2, and params have no effect.
    # MOVE_SEG_QFLUSH  : Flushes all following items in move queue. value1,value2, and params have no effect.
             
    #YAW control: Straight then -90 degree turn 
    #numMoves = 5
    #moveq1 = [numMoves, \
    #    0, 0, 500,   MOVE_SEG_RAMP, 30, 30,  0, STEER_MODE_YAW_SPLIT, int(round(shared.deg2count*0.0)),
    #    60, 60, 2000,   MOVE_SEG_CONSTANT, 0,  0,  0, STEER_MODE_YAW_SPLIT, int(round(shared.deg2count*0.0)),
    #    60, 60, 4000,   MOVE_SEG_CONSTANT, 0,  0,  0, STEER_MODE_YAW_SPLIT, int(round(shared.deg2count*90.0)),
    #    60, 60, 2000,   MOVE_SEG_CONSTANT, 0,  0,  0, STEER_MODE_YAW_SPLIT, int(round(shared.deg2count*90.0)),
    #    60, 60, 500,   MOVE_SEG_RAMP, -30,  -30,  0, STEER_MODE_YAW_SPLIT, int(round(shared.deg2count*90.0))]
    
    #numMoves = 4
    #moveq1 = [numMoves, \
    #    85, 85, 5300,   MOVE_SEG_CONSTANT, 0, 0,  0, STEER_MODE_YAW_DEC, int(round(shared.deg2count*0.0)),
    #    85, 85, 5900,   MOVE_SEG_CONSTANT, 0, 0,  0, STEER_MODE_YAW_DEC, int(round(shared.deg2count*80.0)),
    #    85, 85, 6500,   MOVE_SEG_CONSTANT, 0, 0,  0, STEER_MODE_YAW_DEC, int(round(shared.deg2count*160.0)),
    #    85, 85, 6200,   MOVE_SEG_CONSTANT, 0, 0,  0, STEER_MODE_YAW_DEC, int(round(shared.deg2count*240.0))]
    
    numMoves = 2
    moveq1 = [numMoves, \
        0, 0, 2000, MOVE_SEG_CONSTANT, 0, 0, 0, STEER_MODE_OFF, 0, 
        60, 60, 10000, MOVE_SEG_CONSTANT, 0, 0, 0, STEER_MODE_OFF, 0]
    
    #No movements, just for static telemetry capture
    #numMoves = 1
    #moveq1 = [numMoves, \
    #    0, 0, 2000,   MOVE_SEG_CONSTANT, 0,  0,  0, STEER_MODE_OFF, 0]    
        
    #Timing settings
    R1.leadinTime = 500;
    R1.leadoutTime = 500;
    
    #Flash must be erased to save new data
    if SAVE_DATA1:
        #This needs to be done to prepare the .imudata variables in each robot object
        R1.setupImudata(moveq1)
        R1.eraseFlashMem()

    # Pause and wait to start run, including leadin time
    print ""
    print "  ***************************"
    print "  *******    READY    *******"
    print "  ***************************"
    raw_input("  Press ENTER to start run ...")
    print ""
    
    # Trigger telemetry save, which starts as soon as it is received
    
    #### Make when saving anything, this if is set ####
    #### to the proper "SAVE_DATA"                 ####
    
    if SAVE_DATA1:
        R1.startTelemetrySave()
        
    time.sleep(R1.leadinTime / 1000.0)
    #Send the move queue to the robot; robot will start processing it
    #as soon as it is received
    R1.sendMoveQueue(moveq1)
    
    if SAVE_DATA1:
        maxtime = 0
        for r in shared.ROBOTS:
            tottime =  r.runtime + r.leadoutTime
            if tottime > maxtime:
                maxtime = tottime
    
        #Wait for robots to do runs
        time.sleep(maxtime / 1000.0)
        raw_input("Press Enter to start telemtry readback ...")
        R1.downloadTelemetry()

    if EXIT_WAIT:  #Pause for a Ctrl + Cif specified
        while True:
            try:
                time.sleep(1)
            except KeyboardInterrupt:
                break

    print "Done"
    xb_safe_exit()


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
    #except Exception as args:
    #    print "\nGeneral exception:",args
    #    print "Attemping to exit cleanly..."
    #    shared.xb.halt()
    #    shared.ser.close()
    #    sys.exit()
