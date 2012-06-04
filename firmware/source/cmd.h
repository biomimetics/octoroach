/******************************************************************************
* Name: cmd.h
* Desc: application specific command definitions are defined here.
* Date: 2010-07-10
* Author: stanbaek
******************************************************************************/
#ifndef __CMD_H
#define __CMD_H

#include "cmd_const.h"

#define CMD_SET_THRUST_OPENLOOP     0x80
#define CMD_SET_THRUST_CLOSEDLOOP   0x81
#define CMD_SET_PID_GAINS  			0x82
#define CMD_GET_PID_TELEMETRY		0x83
#define CMD_SET_CTRLD_TURN_RATE		0x84
#define CMD_GET_IMU_LOOP_ZGYRO      0x85
#define CMD_SET_MOVE_QUEUE	        0x86
#define CMD_SET_STEERING_GAINS      0x87
#define CMD_SOFTWARE_RESET          0x88
#define CMD_SPECIAL_TELEMETRY       0x89
#define CMD_ERASE_SECTORS           0x8A
#define CMD_FLASH_READBACK          0x8B
# define CMD_SET_VEL_PROFILE	0x8C
#define CMD_WHO_AM_I 0x8D
#define CMD_START_TELEM 0x8E
#define CMD_ZERO_POS 0x8f

void cmdSetup(void);
void cmdHandleRadioRxBuffer(void);
void cmdEcho(unsigned char status, unsigned char length, unsigned char *frame);


#endif // __CMD_H

