/******************************************************************************
* Name: cmd.h
* Desc: application specific command definitions are defined here.
* Date: 2010-07-10
* Author: stanbaek
******************************************************************************/
#ifndef __CMD_H
#define __CMD_H

#include "cmd_const.h"

//// Includes here should be to provide TYPES and ENUMS only
#include "move_queue.h"
#include "tail_queue.h"
#include "hall.h"

#define CMD_VECTOR_SIZE				0xFF //full length vector
#define MAX_CMD_FUNC				0x9F

#define CMD_SET_THRUST_OPENLOOP     0x80
#define CMD_SET_THRUST_CLOSEDLOOP   0x81
#define CMD_SET_PID_GAINS           0x82
#define CMD_GET_PID_TELEMETRY       0x83
#define CMD_SET_CTRLD_TURN_RATE     0x84
#define CMD_STREAM_TELEMETRY        0x85
#define CMD_SET_MOVE_QUEUE	    0x86
#define CMD_SET_STEERING_GAINS      0x87
#define CMD_SOFTWARE_RESET          0x88
#define CMD_SPECIAL_TELEMETRY       0x89
#define CMD_ERASE_SECTORS           0x8A
#define CMD_FLASH_READBACK          0x8B
#define CMD_SLEEP                   0x8C
#define CMD_SET_VEL_PROFILE         0x8D
#define CMD_WHO_AM_I                0x8E
#define CMD_HALL_TELEMETRY          0x8F
#define CMD_ZERO_POS                0x90
#define CMD_SET_HALL_GAINS          0x91
#define CMD_SET_TAIL_QUEUE          0x92
#define CMD_SET_TAIL_GAINS          0x93
#define CMD_SET_THRUST_HALL         0x94

//Argument lengths
//lenghts are in bytes
//OBSOLETE, replaced with command-specific structs
/*
#define LEN_CMD_SET_THRUST_OPENLOOP     4   //2 unsigned int
#define LEN_CMD_SET_THRUST_CLOSEDLOOP   10  //5 unsigned int
#define LEN_CMD_SET_PID_GAINS  		20  //10 unsigned int
#define LEN_CMD_GET_PID_TELEMETRY	2   //1 unsigned int
#define LEN_CMD_SET_CTRLD_TURN_RATE	2   //1 signed int
#define LEN_CMD_GET_IMU_LOOP_ZGYRO      2   //1 unsigned int
#define LEN_CMD_SET_MOVE_QUEUE	        -1  //variable length
#define LEN_CMD_SET_STEERING_GAINS      12  //6 signed int
#define LEN_CMD_SOFTWARE_RESET          1   //1 char
#define LEN_CMD_SPECIAL_TELEMETRY       4   //1 unsigned long
#define LEN_CMD_ERASE_SECTORS           4   //1 unsigned long
#define LEN_CMD_FLASH_READBACK          4   //1 unsigned long
#define LEN_CMD_SLEEP			1   //1 char
*/

void cmdSetup(void);
void cmdHandleRadioRxBuffer(void);
void cmdEcho(unsigned char status, unsigned char length, unsigned char *frame);



/////// Argument structures

//cmdSetThrustOpenLoop
typedef struct{
	int dc1, dc2;
} _args_cmdSetThrustOpenLoop;

//cmdSetThrustClosedLoop
typedef struct{
	int chan1;
        unsigned int runtime1;
        int chan2;
        unsigned int runtime2;
        unsigned int telem_samples;
} _args_cmdSetThrustClosedLoop;

//cmdSetPIDGains
typedef struct{
	int Kp1, Ki1, Kd1, Kaw1, Kff1;
	int Kp2, Ki2, Kd2, Kaw2, Kff2;
} _args_cmdSetPIDGains;

//cmdGetPIDTelemetry
//obsolete

//cmdSetCtrldTurnRate
typedef struct{
	int steerInput;
} _args_cmdSetCtrldTurnRate;

//cmdStreamTelemetry
typedef struct{
    unsigned long count;
} _args_cmdStreamTelemetry;

//cmdSetMoveQueue
//NOTE: This is not for the entire packet, just for one moveQ items,
// the cmd handler will stride across the packet, unpacking these
typedef struct{
	int inputL, inputR;
	unsigned long duration;
	enum moveSegT type;
	int params[3];
} _args_cmdSetMoveQueue;

//cmdSetSteeringGains
typedef struct{
	int Kp, Ki, Kd, Kaw, Kff;
        int steerMode;
} _args_cmdSetSteeringGains;

//cmdSoftwareReset
//no arguments

//cmdSpecialTelemetry
typedef struct{
	unsigned long count;
} _args_cmdSpecialTelemetry;

//cmdEraseSector
typedef struct{
	unsigned long samples;
} _args_cmdEraseSector;

//cmdFlashReadback
typedef struct{
	unsigned long samples;
} _args_cmdFlashReadback;

//cmdSleep

//cmdSetVelProfile
typedef struct{
    int intervalsL[NUM_VELS];
    int deltaL[NUM_VELS];
    int velL[NUM_VELS];
    int intervalsR[NUM_VELS];
    int deltaR[NUM_VELS];
    int velR[NUM_VELS];
} _args_cmdSetVelProfile;

//cmdHallTelemetry
typedef struct {
    unsigned long startDelay; // recording start time
    int count; // count of samples to record
    int skip; // samples to skip
} _args_cmdHallTelemetry;

//cmdSetTailQueue
//NOTE: This is not for the entire packet, just for one tailQ item,
// the cmd handler will stride across the packet, unpacking these
typedef struct {
    float angle;
    unsigned long duration;
    enum tailSegT type;
    int params[3];
} _args_cmdSetTailQueue;

//cmdSetTailGains
typedef struct{
	int Kp, Ki, Kd, Kaw, Kff;
} _args_cmdSetTailGains;

//cmdSetThrustHall
typedef struct{
	int chan1;
        unsigned int runtime1;
        int chan2;
        unsigned int runtime2;
} _args_cmdSetThrustHall;

#endif // __CMD_H

