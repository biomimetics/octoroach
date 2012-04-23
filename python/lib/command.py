#!/usr/bin/env python
"""
cmd module
authors: stanbaek, apullin
Created on 2010-07-07:
Modified by Andrew Pullin for OctoROACH

"""

# CMD values of 0x00 - 0x7F(127) are defined here
# Add CMD definitions 
# for bootloader (0x00 - 0x1F)
CMD_NACK = 0x00        # START_APPLICATION = 0
CMD_ACK = 0x01
CMD_READ_PM = 0x02
CMD_WRITE_PM = 0x03
CMD_READ_EE = 0x04
CMD_WRITE_EE = 0x05
CMD_READ_CM = 0x06
CMD_WRITE_CM = 0x07
CMD_RESET = 0x08
CMD_READ_ID = 0x09
CMD_READ_GOTO = 0x10

SET_THRUST = 0x11
SET_STEER = 0x12
ECHO = 0x1F      # send back the received packet

# for IMU (0x20 - 0x3F)
GET_IMU_DATA = 0x20
GET_IMU_LOOP = 0x21
START_IMU_SAVE = 0x22
STOP_IMU_SAVE = 0x23

SET_POSE_SAVE_FLASH = 0x25
SET_ESTIMATE_POSE = 0x26

TX_SAVED_IMU_DATA = 0x2A
TX_SAVED_STATE_DATA = 0x2B
TX_DUTY_CYCLE = 0x2C

START_AUTO_CTRL = 0x30
STOP_AUTO_CTRL = 0x31

ERASE_MEM_SECTOR = 0x3A
 
RESET_STOPWATCH = 0x3B

BASE_ECHO = 0x3f


# CMD values of 0x80(128) - 0xEF(239) are available for user applications.
SET_THRUST_OPEN_LOOP =      0x80
SET_THRUST_CLOSED_LOOP =    0x81
SET_PID_GAINS =             0x82
GET_PID_TELEMETRY =         0x83
SET_CTRLD_TURN_RATE =       0x84
GET_IMU_LOOP_ZGYRO =        0x85
SET_MOVE_QUEUE =            0x86
SET_STEERING_GAINS =        0x87
SOFTWARE_RESET     =        0x88
SPECIAL_TELEMETRY  =        0x89
ERASE_SECTORS      =        0x8A
FLASH_READBACK     =        0x8B
SLEEP              =        0x8C

# CMD values of 0xF0(240) - 0xFF(255) are reserved for future use
