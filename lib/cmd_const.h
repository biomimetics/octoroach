/******************************************************************************
* Name: cmd_const.h
* Desc: Commonly used command definitions are defined here.
* Date: 2010-07-10
* Author: stanbaek
******************************************************************************/
#ifndef __CMD_CONST_H
#define __CMD_CONST_H

// CMD values of 0x00(0) - 0x3F(127) are defined here

// for bootloader (0x00 - 0x1F)
#define CMD_NACK                    0x00
#define CMD_ACK                     0x01
#define CMD_READ_PM                 0x02
#define CMD_WRITE_PM                0x03
#define CMD_WRITE_CM                0x07
#define CMD_RESET                   0x08
#define CMD_READ_ID                 0x09
#define CMD_READ_GOTO               0x10


#define CMD_SET_THRUST              0x11
#define CMD_SET_STEER               0x12

#define CMD_ECHO                    0x1F      // send back the received packet

// for IMU 
#define CMD_GET_IMU_DATA            0x20
#define CMD_GET_IMU_LOOP            0x21
#define CMD_START_IMU_SAVE          0x22
#define CMD_STOP_IMU_SAVE           0x23

#define CMD_SET_POSE_SAVE_FLASH     0x25
#define CMD_SET_ESTIMATE_POSE       0x26

#define CMD_TX_SAVED_IMU_DATA       0x2A
#define CMD_TX_SAVED_STATE_DATA     0x2B
#define CMD_TX_DUTY_CYCLE           0x2C

#define CMD_START_AUTO_CTRL         0x30
#define CMD_STOP_AUTO_CTRL          0x31


#define CMD_ERASE_MEM_SECTOR        0x3A

#define CMD_RESET_STOPWATCH         0x3B

#define CMD_BASE_ECHO               0x3F        // send back the received packet





/*
#define CMD_DCSWEEP         106
#define CMD_DIRDCUPDATE     110
#define CMD_STEERUPDATE     102
#define CMD_TURNLEFT        103
#define CMD_TURNRIGHT       104
#define CMD_NOTURN          105
#define CMD_DIRECTION       107
#define CMD_DIRFORWARD      108
#define CMD_DIRBACKWARD     109
#define CMD_WIIMOTE         121
#define CMD_WIIREAD         122
#define CMD_WIICONTREAD     123
#define CMD_WIIBLOBREAD     124
#define CMD_AUTOPILOT       131
#define CMD_CAPTUREDATA     132
#define CMD_SETGAINS        141
*/




// CMD values of 0x80(128) - 0xEF(239) are available for user applications.
// Create a header file in your project such as cmd.h, and add 
// #include "cmd_common.h" to the header file.
// Other command definitions can be defined in cmd.h.


// CMD values of 0xF0(240) - 0xFF(255) are reserved for future use


#endif  // __CMD_CONST_H

