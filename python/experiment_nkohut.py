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
    
    

    #Motor gains format:
    #  [ Kp , Ki , Kd , Kaw , Kff     ,  Kp , Ki , Kd , Kaw , Kff ]
    #    ----------LEFT----------        ---------_RIGHT----------
    
    motorgains = [8000,100,2,0,0 , 8000,100,2,0,0] #Hardware PID
    #motorgains = [200,2,0,2,0,    200,2,0,2,0]       #Software PID
    #motorgains = [0000,000,0,0,0 , 0000,000,0,0,0]
    setMotorGains(motorgains)

    #Steering gains format:
    #  [ Kp , Ki , Kd , Kaw , Kff]
    #
    #steeringGains = [5000,0,0,0,0,  STEER_MODE_DECREASE] # Disables steering controller
    #steeringGains = [20,1,0,1,0,  STEER_MODE_DECREASE]
    steeringGains = [50,10,0,0,0,  STEER_MODE_DECREASE] # Hardware PID
    #steeringGains = [0000,0,0,0,0,  STEER_MODE_DECREASE] 
    setSteeringGains(steeringGains)
    setSteeringRate(0)

    #Tail gains format:
    #  [ Kp , Ki , Kd , Kaw , Kff]
    #
    tailGains = [8000,20,1000,0,0] #tuned 7/12/12
    #tailGains = [0,0,0,0,0] # Disables tail controller
    setTailGains(tailGains)
    time.sleep(1)


    

    # Gyro Control Experiment ###############################

    #exp_time = 1000

    #start_angle = -90

    #throttle = 300

    #tailq_init = [2, \
    #         0, exp_time, TAIL_SEG_CONSTANT, 0, 0, 0,
    #         0, exp_time, TAIL_SEG_RAMP, start_angle*10, 0, 0
    #         ]
    
    #moves = 3
    #moveq = [moves, \
    #         throttle, throttle, exp_time, MOVE_SEG_CONSTANT, 0, 0, 0, STEER_MODE_DECREASE, 0,
    #         throttle, throttle, exp_time/2, MOVE_SEG_CONSTANT, 0, 0, 0, STEER_MODE_OFF, 0,
    #         throttle, throttle, exp_time/2, MOVE_SEG_CONSTANT, 0, 0, 0, STEER_MODE_DECREASE, 0
    #         ]

 

    # Tail range of motion is between 140 and -125 deg! do not exceed
    #tailq = [moves, \
    #         start_angle, exp_time, TAIL_SEG_CONSTANT, 0, 0, 0,
    #         0, exp_time/2, TAIL_GYRO_CONTROL, -90, 0, 0,
    #         0, exp_time/2, TAIL_SEG_IDLE, 0, 0, 0
    #         ]

#################################################33


# Friction Experiments

    exp_time = 1000

    tailTime = 150

    start_angle = -90

    throttle = 0

    init_moves = 2

    tailq_init = [init_moves, \
            0, exp_time, TAIL_SEG_CONSTANT, 0, 0, 0,
            0, exp_time, TAIL_SEG_RAMP, start_angle*10, 0, 0
             ]

    moveq_init = [init_moves, \
             throttle, throttle, exp_time, MOVE_SEG_CONSTANT, 0, 0, 0, STEER_MODE_DECREASE, 0,
             throttle, throttle, exp_time, MOVE_SEG_CONSTANT, 0, 0, 0, STEER_MODE_DECREASE, 0
             ]
    
    moves = 3
    moveq = [moves, \
             throttle, throttle, exp_time, MOVE_SEG_CONSTANT, 0, 0, 0, STEER_MODE_DECREASE, 0,
             throttle, throttle, tailTime, MOVE_SEG_CONSTANT, 0, 0, 0, STEER_MODE_OFF, 0,
             throttle, throttle, exp_time/2, MOVE_SEG_CONSTANT, 0, 0, 0, STEER_MODE_OFF, 0
             ]

 

    # Tail range of motion is between 140 and -125 deg! do not exceed
    tailq = [moves, \
             start_angle, exp_time, TAIL_SEG_CONSTANT, 0, 0, 0,
             0, tailTime, TAIL_GYRO_CONTROL, -100, 0, 0,
             0, exp_time/2, TAIL_SEG_IDLE, 0, 0, 0
             ]

    ######################################3


    # Friction Experiments - going straight up hill

    #exp_time = 5000

   

    #start_angle = -90

    #throttle = 400

    
    
    #moves = 1
    #moveq = [moves, \
     #        throttle, throttle, exp_time, MOVE_SEG_CONSTANT, 0, 0, 0, STEER_MODE_DECREASE,0
    #                ]

 

    ######################################3



    #moves = 2
    
    #moveq = [moves, \
     #        300, 300, exp_time, MOVE_SEG_CONSTANT, 0, 0, 0, STEER_MODE_DECREASE, 0,
     #        200, 200, exp_time, MOVE_SEG_CONSTANT, 0, 0, 0, STEER_MODE_DECREASE, 20]

    

    #tailq = [tmoves, \
    #         0, exp_time, TAIL_GYRO_CONTROL, -20, 0, 0
    #         ]

    #tailq = [tmoves, \
    #         90.0, 6000, MOVE_SEG_RAMP, -30*10, 0, 0,
     #        -90.0, 6000, MOVE_SEG_RAMP, 30*10, 0, 0
     #        ]
    
    
      
    #Ramp example
    #moves = 5
    #moveq = [moves, \
    #    0,   0,   0,   MOVE_SEG_LOOP_DECL,    0, 0, 0,
    #    0,   0,   500,   MOVE_SEG_RAMP,    300, 300, 0,
    #    150, 150, 1000,   MOVE_SEG_CONSTANT, 0,  0,  0,
    #    150, 150, 500,   MOVE_SEG_RAMP, -300,  -300,  0,
    #    0, 0, 100,   MOVE_SEG_CONSTANT, 0,  0,  0]

    #Sin example
    #RAD_TO_BAMS16 = (0x7FFF)/(3.1415)
    #phase = 3.1415/2 * RAD_TO_BAMS16
    #moves = 2
    #moveq = [moves, \
    #         76,   76,   2000,   MOVE_SEG_SIN,  75, 1000, phase,
    #	     75, 75, 2000,   MOVE_SEG_CONSTANT, 0,  0,  0]
    
    shared.runtime = exp_time*moves
    
    #Timing settings
    shared.leadinTime = 500;
    shared.leadoutTime = 500;
    
    numSamples = calcNumSamples(moveq)
    #numSamples = int(floor((tailq[2]+shared.leadinTime+shared.leadoutTime)*0.150))
    
    shared.imudata = [ [] ] * numSamples
    
    #Flash must be erased to save new data
    if SAVE_DATA:
        eraseFlashMem(numSamples)

    # Pause and wait to start run, including leadin time
    raw_input("Press enter to start run ...")
    
    sendTailQueue(tailq_init)
    #sendMoveQueue(moveq_init)
    time.sleep(2)
    # Trigger telemetry save, which starts as soon as it is received
    if SAVE_DATA:
        startTelemetrySave(numSamples)

    time.sleep(shared.leadinTime / 1000.0)
    #Send the move queue to the robot; robot will start processing it
    #as soon as it is received
    
    #sendMoveQueue(moveq)
    sendTailQueue(tailq)
    
    #Clear loop
    #time.sleep(6)
    #mqclear = [1, 0,   0,   0,   MOVE_SEG_LOOP_CLEAR,    0, 0, 0]
    #mqflush = [1, 0,   0,   0,   MOVE_SEG_QFLUSH,    0, 0, 0]

    if SAVE_DATA:
        #print "Sending loop clear!!"
        #sendMoveQueue(mqclear)
        time.sleep(0.05)
        #print "Sending move queue flush!!"
        #sendMoveQueue(mqflush)
        
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
