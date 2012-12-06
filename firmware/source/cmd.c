/***************************************************************************
 * Name: cmd.c
 * Desc: Receiving and transmitting queue handler
 * Date: 2010-07-10
 * Author: stanbaek
 **************************************************************************/

#include "cmd.h"
#include "cmd_const.h"
#include "dfmem.h"
#include "utils.h"
#include "ports.h"
#include "gyro.h"
#include "xl.h"
#include "sclock.h"
#include "led.h"
#include "motor_ctrl.h"
#include "payload.h"
#include "sensors.h"
#include "dfmem.h"
#include "pid.h"
#include "radio.h"
#include "move_queue.h"
#include "steering.h"
#include "telem.h"
#include "leg_ctrl.h"
#include "tail_ctrl.h"
#include "hall.h"
#include "version.h"

#include "settings.h" //major config defines, sys-service, hall, etc

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PKT_UNPACK(type, var, pktframe) type* var = (type*)(pktframe);

unsigned char tx_frame_[127];

extern MoveQueue moveq;
extern TailQueue tailq;
extern int offsz;

extern moveCmdT currentMove, idleMove;
extern tailCmdT currentTail, idleTail;

extern volatile char g_radio_duty_cycle;

// use an array of function pointer to avoid a number of case statements
// CMD_VECTOR_SIZE is defined in cmd_const.h
void (*cmd_func[CMD_VECTOR_SIZE])(unsigned char, unsigned char, unsigned char*);
//char cmd_len[CMD_VECTOR_SIZE];

/*-----------------------------------------------------------------------------
 *          Declaration of static functions
-----------------------------------------------------------------------------*/
static void cmdSetThrust(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSteer(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdGetImuData(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdGetImuLoop(unsigned char status, unsigned char length, unsigned char *frame);

static void cmdStartImuDataSave(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdStopImuDataSave(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdTxSavedImuData(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdEraseMemSector(unsigned char status, unsigned char length, unsigned char *frame);

//static void cmdEcho(unsigned char status, unsigned char length, unsigned char *frame);
//void cmdEcho(unsigned char status, unsigned char length, unsigned char *frame);

static void cmdNop(unsigned char status, unsigned char length, unsigned char *frame);

//User commands
static void cmdSetThrustOpenLoop(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSetThrustClosedLoop(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSetPIDGains(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdGetPIDTelemetry(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSetCtrldTurnRate(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdGetImuLoopZGyro(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSetMoveQueue(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSetSteeringGains(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSoftwareReset(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSpecialTelemetry(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdEraseSector(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdFlashReadback(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSleep(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSetVelProfile(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdWhoAmI(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdHallTelemetry(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdZeroPos(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSetHallGains(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSetTailQueue(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSetTailGains(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSetThrustHall(unsigned char status, unsigned char length, unsigned char *frame);

/*-----------------------------------------------------------------------------
 *          Public functions
-----------------------------------------------------------------------------*/
void cmdSetup(void) {

    unsigned int i;

    // initialize the array of func pointers with Nop()
    for (i = 0; i < MAX_CMD_FUNC; ++i) {
        cmd_func[i] = &cmdNop;
        //cmd_len[i] = 0; //0 indicated an unpoplulated command
    }

    cmd_func[CMD_ECHO] = &cmdEcho;
    cmd_func[CMD_SET_THRUST] = &cmdSetThrust;
    cmd_func[CMD_SET_STEER] = &cmdSteer;
    cmd_func[CMD_GET_IMU_DATA] = &cmdGetImuData;
    cmd_func[CMD_GET_IMU_LOOP] = &cmdGetImuLoop;
    cmd_func[CMD_START_IMU_SAVE] = &cmdStartImuDataSave;
    cmd_func[CMD_STOP_IMU_SAVE] = &cmdStopImuDataSave;
    cmd_func[CMD_TX_SAVED_IMU_DATA] = &cmdTxSavedImuData;
    cmd_func[CMD_ERASE_MEM_SECTOR] = &cmdEraseMemSector;
    //User commands
    cmd_func[CMD_SET_THRUST_OPENLOOP] = &cmdSetThrustOpenLoop;
    cmd_func[CMD_SET_THRUST_CLOSEDLOOP] = &cmdSetThrustClosedLoop;
    cmd_func[CMD_SET_PID_GAINS] = &cmdSetPIDGains;
    cmd_func[CMD_GET_PID_TELEMETRY] = &cmdGetPIDTelemetry;
    cmd_func[CMD_SET_CTRLD_TURN_RATE] = &cmdSetCtrldTurnRate;
    cmd_func[CMD_STREAM_TELEMETRY] = &cmdGetImuLoopZGyro;
    cmd_func[CMD_SET_MOVE_QUEUE] = &cmdSetMoveQueue;
    cmd_func[CMD_SET_STEERING_GAINS] = &cmdSetSteeringGains;
    cmd_func[CMD_SOFTWARE_RESET] = &cmdSoftwareReset;
    cmd_func[CMD_SPECIAL_TELEMETRY] = &cmdSpecialTelemetry;
    cmd_func[CMD_ERASE_SECTORS] = &cmdEraseSector;
    cmd_func[CMD_FLASH_READBACK] = &cmdFlashReadback;
    cmd_func[CMD_SLEEP] = &cmdSleep;
    cmd_func[CMD_SET_VEL_PROFILE] = &cmdSetVelProfile;
    cmd_func[CMD_WHO_AM_I] = &cmdWhoAmI;
    cmd_func[CMD_HALL_TELEMETRY] = &cmdHallTelemetry;
    cmd_func[CMD_ZERO_POS] = &cmdZeroPos;
    cmd_func[CMD_SET_HALL_GAINS] = &cmdSetHallGains;
    cmd_func[CMD_SET_TAIL_QUEUE] = &cmdSetTailQueue;
    cmd_func[CMD_SET_TAIL_GAINS] = &cmdSetTailGains;
    cmd_func[CMD_SET_THRUST_HALL] = &cmdSetThrustHall;

    //Set up command length vector
    /*cmd_len[CMD_SET_THRUST_OPENLOOP] = LEN_CMD_SET_THRUST_OPENLOOP;
    cmd_len[CMD_SET_THRUST_CLOSEDLOOP] = LEN_CMD_SET_THRUST_CLOSEDLOOP;
    cmd_len[CMD_SET_PID_GAINS] = LEN_CMD_SET_PID_GAINS;
    cmd_len[CMD_GET_PID_TELEMETRY] = LEN_CMD_GET_PID_TELEMETRY;
    cmd_len[CMD_SET_CTRLD_TURN_RATE] = LEN_CMD_SET_CTRLD_TURN_RATE;
    cmd_len[CMD_GET_IMU_LOOP_ZGYRO] = LEN_CMD_GET_IMU_LOOP_ZGYRO;
    cmd_len[CMD_SET_MOVE_QUEUE] = LEN_CMD_SET_MOVE_QUEUE;
    cmd_len[CMD_SET_STEERING_GAINS] = LEN_CMD_SET_STEERING_GAINS;
    cmd_len[CMD_SOFTWARE_RESET] = LEN_CMD_SOFTWARE_RESET;
    cmd_len[CMD_SPECIAL_TELEMETRY] = LEN_CMD_SPECIAL_TELEMETRY;
    cmd_len[CMD_ERASE_SECTORS] = LEN_CMD_ERASE_SECTORS;
    cmd_len[CMD_FLASH_READBACK] = LEN_CMD_FLASH_READBACK;
    cmd_len[CMD_SLEEP] = LEN_CMD_SLEEP;
     * */
}

void cmdHandleRadioRxBuffer(void) {

    Payload pld;
    unsigned char command, status;

    if ((pld = radioReceivePayload()) != NULL) {

        status = payGetStatus(pld);
        command = payGetType(pld);

        //Due to bugs, command may be a surprious value; check explicitly
        if (command <= MAX_CMD_FUNC) {
            cmd_func[command](status, pld->data_length, payGetData(pld));
        }

        payDelete(pld);
    }

    return;
}

//////////////////////////////////
//typedef struct { int dc1, dc2;} _args_cmdSetThrustOpenLoop;
/////////////////////////////////

/*-----------------------------------------------------------------------------
 * ----------------------------------------------------------------------------
 * The functions below are intended for internal use, i.e., private methods.
 * Users are recommended to use functions defined above.
 * ----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

static void cmdSetThrust(unsigned char status, unsigned char length, unsigned char *frame) {

    unsigned char chr_test[4];
    float *duty_cycle = (float*) chr_test;

    chr_test[0] = frame[0];
    chr_test[1] = frame[1];
    chr_test[2] = frame[2];
    chr_test[3] = frame[3];

    mcSetDutyCycle(MC_CHANNEL_PWM1, duty_cycle[0]);
    //mcSetDutyCycle(1, duty_cycle[0]);
}

static void cmdSteer(unsigned char status, unsigned char length, unsigned char *frame) {

    unsigned char chr_test[4];
    float *steer_value = (float*) chr_test;

    chr_test[0] = frame[0];
    chr_test[1] = frame[1];
    chr_test[2] = frame[2];
    chr_test[3] = frame[3];

    mcSteer(steer_value[0]);
}

/*-----------------------------------------------------------------------------
 *          IMU functions
-----------------------------------------------------------------------------*/
static void cmdGetImuData(unsigned char status, unsigned char length, unsigned char *frame) {

	senGetIMUData(status, CMD_GET_IMU_DATA);
    /*unsigned char *xl_data, *gyro_data;
    unsigned char i;
    Payload pld = payCreateEmpty(14);  // data length = 12

    xlReadXYZ();
    xl_data = xlGetsXYZ();
    gyroReadXYZ();
    gyro_data = gyroGetsXYZ();
    for(i = 0; i < 6; ++i) {
        pld->pld_data[i] = xl_data[i];
        pld->pld_data[i+6] = gyro_data[i];
    }

    pld->status = status;
    pld->type = CMD_GET_IMU_DATA;
    radioTxPayload(pld);
    */
}


// return packet format:
// 4 bytes for time
// 6 bytes for xl data
// 6 bytes for gyro data

static void cmdGetImuLoop(unsigned char status, unsigned char length, unsigned char *frame) {

    unsigned int count;
    unsigned long tic;
    unsigned char *tic_char;
    Payload pld;

    LED_RED = 1;

    count = frame[0] + (frame[1] << 8);

    tic_char = (unsigned char*) &tic;
    tic = sclockGetTime();

    while (count) {

        pld = payCreateEmpty(16); // data length = 16
        paySetData(pld, 4, tic_char);
        payAppendData(pld, 4, 6, xlReadXYZ());
        payAppendData(pld, 10, 6, gyroReadXYZ());
        paySetStatus(pld, status);
        paySetType(pld, CMD_GET_IMU_DATA);

        radioSendPayload(macGetDestAddr(), pld);
        count--;
        payDelete(pld);
        delay_ms(4);
        tic = sclockGetTime();
    }

    LED_RED = 0;

}

static void cmdStartImuDataSave(unsigned char status, unsigned char length, unsigned char *frame) {
    
}

static void cmdStopImuDataSave(unsigned char status, unsigned char length, unsigned char *frame) {
    
}

static void cmdTxSavedImuData(unsigned char status, unsigned char length, unsigned char *frame) {

    unsigned int page, byte;
    unsigned int i, j;
    Payload pld;

    //senGetMemLocIndex(&page, &byte);
    page = 0x0200;
    byte = 0;

    LED_RED = 1;

    dfmemEraseSector(0x0100); // erase Sector 1 (page 256 - 511)

    for (i = 0x0100; i < 0x0200; ++i) {
        j = 0;
        while (j < 512) {
            pld = payCreateEmpty(18); // data length = 16
            dfmemRead(i, j, 16, pld->pld_data);
            paySetStatus(pld, status);
            paySetType(pld, CMD_GET_IMU_DATA);
            while (!radioReceivePayload());
            j += 16;
        }
        delay_ms(200);
    }

    LED_RED = 0;
}

static void cmdEraseMemSector(unsigned char status, unsigned char length, unsigned char *frame) {
    unsigned int page;
    page = frame[0] + (frame[1] << 8);
    LED_RED = 1;
    dfmemEraseSector(0x0100); // erase Sector 1 (page 256 - 511)
    LED_RED = 0;
}

/*-----------------------------------------------------------------------------
 *          AUX functions
-----------------------------------------------------------------------------*/
void cmdEcho(unsigned char status, unsigned char length, unsigned char *frame) {

    char* temp = malloc(length*sizeof(char));
    strncpy(temp,(char*)frame,length);

    radioSendPayload(macGetDestAddr(), payCreate(length, frame, status, CMD_ECHO));
    //Payload pld;
    //pld = payCreateEmpty(1);
    //paySetStatus(pld, status);
    //paySetType(pld, CMD_ECHO);
    //unsigned char temp = 99;
    //paySetData(pld, 1, &temp);

    //radioSendPayload((WordVal) macGetDestAddr(), pld);
}

static void cmdNop(unsigned char status, unsigned char length, unsigned char *frame) {
    Nop();
}

/*-----------------------------------------------------------------------------
 *         User function
-----------------------------------------------------------------------------*/
static void cmdSetThrustOpenLoop(unsigned char status, unsigned char length, unsigned char *frame) {
    PKT_UNPACK(_args_cmdSetThrustOpenLoop, argsPtr, frame);

    //set motor duty cycles
    //PDC1 = argsPtr->dc1;
    //PDC2 = argsPtr->dc1;
    mcSetDutyCycle(MC_CHANNEL_PWM1, argsPtr->dc1);
    mcSetDutyCycle(MC_CHANNEL_PWM2, argsPtr->dc2);
}

static void cmdSetThrustClosedLoop(unsigned char status, unsigned char length, unsigned char *frame) {
    PKT_UNPACK(_args_cmdSetThrustClosedLoop, argsPtr, frame);

    legCtrlSetInput(LEG_CTRL_LEFT, argsPtr->chan1);
    legCtrlOnOff(LEG_CTRL_LEFT, PID_ON); //Motor PID #1 -> ON

    legCtrlSetInput(LEG_CTRL_RIGHT, argsPtr->chan2);
    legCtrlOnOff(LEG_CTRL_RIGHT, PID_ON); //Motor PID #2 -> ON
}

static void cmdSetPIDGains(unsigned char status, unsigned char length, unsigned char *frame) {
    PKT_UNPACK(_args_cmdSetPIDGains, argsPtr, frame);

    legCtrlSetGains(0, argsPtr->Kp1, argsPtr->Ki1, argsPtr->Kd1, argsPtr->Kaw1, argsPtr->Kff1);
    legCtrlSetGains(1, argsPtr->Kp2, argsPtr->Ki2, argsPtr->Kd2, argsPtr->Kaw2, argsPtr->Kff2);

    //Send confirmation packet
    Payload pld;
    pld = payCreateEmpty(20);
    pld->pld_data[0] = status;
    pld->pld_data[1] = CMD_SET_PID_GAINS;
    memcpy((pld->pld_data) + 2, frame, 20);
    radioSendPayload((WordVal) macGetDestAddr(), pld);
}

static void cmdGetPIDTelemetry(unsigned char status, unsigned char length, unsigned char *frame) {
    //Obsolete, not maintained
   
}

static void cmdSetCtrldTurnRate(unsigned char status, unsigned char length, unsigned char *frame) {
    Payload pld;
    PKT_UNPACK(_args_cmdSetCtrldTurnRate, argsPtr, frame);
    steeringSetInput(argsPtr->steerInput);

    pld = payCreateEmpty(sizeof(_args_cmdSetCtrldTurnRate));
    pld->pld_data[0] = status;
    pld->pld_data[1] = CMD_SET_CTRLD_TURN_RATE;
    memcpy((pld->pld_data) + 2, frame, sizeof(_args_cmdSetCtrldTurnRate));
    radioSendPayload((WordVal) macGetDestAddr(), pld);
}

static void cmdGetImuLoopZGyro(unsigned char status, unsigned char length, unsigned char *frame) {
    //Obsolete, do not use
}

static void cmdSetMoveQueue(unsigned char status, unsigned char length, unsigned char *frame) {
    //The count is read via standard pointer math
    unsigned int count;
    count = (unsigned int) (*(frame));

    //Unpack unsigned char* frame into structured values
    //Here, unpacking happens in the loop below.
    //_args_cmdSetMoveQueue* argsPtr;
    int idx = sizeof (count); //should be an unsigned int, sizeof(uint) = 2

    moveCmdT move;
    int i;
    for (i = 0; i < count; i++) {
        move = (moveCmdT) malloc(sizeof (moveCmdStruct));
        //argsPtr = (_args_cmdSetMoveQueue*)(frame+idx);
        *move = *((moveCmdT) (frame + idx));
        mqPush(moveq, move);
        //idx =+ sizeof(_args_cmdSetMoveQueue);
        idx += sizeof (moveCmdStruct);
    }
}

//Format for steering gains:
// [Kp, Ki, Kd, Kaw, Kff, steeringMode]
//

static void cmdSetSteeringGains(unsigned char status, unsigned char length, unsigned char *frame) {
    Payload pld;

    PKT_UNPACK(_args_cmdSetSteeringGains, argsPtr, frame);

    steeringSetGains(argsPtr->Kp, argsPtr->Ki, argsPtr->Kd, argsPtr->Kaw, argsPtr->Kff);
    steeringSetMode(argsPtr->steerMode);

    pld = payCreateEmpty(sizeof(_args_cmdSetSteeringGains));
    pld->pld_data[0] = status;
    pld->pld_data[1] = CMD_SET_STEERING_GAINS;
    memcpy((pld->pld_data) + 2, frame, sizeof(_args_cmdSetSteeringGains));
    radioSendPayload((WordVal) macGetDestAddr(), pld);

}

static void cmdSoftwareReset(unsigned char status, unsigned char length, unsigned char *frame) {
    char* resetmsg = "RESET";
    radioSendPayload(macGetDestAddr(), payCreate(6, (unsigned char*)resetmsg, status, CMD_ECHO));
    delay_ms(10);
#ifndef __DEBUG
    __asm__ volatile ("reset");
#endif
}

static void cmdSpecialTelemetry(unsigned char status, unsigned char length, unsigned char *frame) {
    PKT_UNPACK(_args_cmdSpecialTelemetry, argsPtr, frame);

    if (argsPtr->count != 0) {
        telemSetStartTime(); // Start telemetry samples from approx 0 time
        telemSetSamplesToSave(argsPtr->count);
    }
}

static void cmdEraseSector(unsigned char status, unsigned char length, unsigned char *frame) {
    PKT_UNPACK(_args_cmdEraseSector, argsPtr, frame);

    telemErase(argsPtr->samples);

    //Send a confirmation packet
    Payload pld;
    pld = payCreateEmpty(4);
    paySetData(pld, 4, (unsigned char*) (&(argsPtr->samples)));
    paySetStatus(pld, status);
    paySetType(pld, CMD_ERASE_SECTORS);
    radioSendPayload((WordVal) macGetDestAddr(), pld);
}

static void cmdFlashReadback(unsigned char status, unsigned char length, unsigned char *frame) {
    LED_YELLOW = 1;
    PKT_UNPACK(_args_cmdFlashReadback, argsPtr, frame);

    telemReadbackSamples(argsPtr->samples);
}

static void cmdSleep(unsigned char status, unsigned char length, unsigned char *frame) {
    char sleep = frame[0];
    if (sleep) {
        //g_radio_duty_cycle = 1;
    } else {
        //g_radio_duty_cycle = 0;
        Payload pld;
        pld = payCreateEmpty(1);
        paySetData(pld, 1, (unsigned char*) (&sleep)); //echo back a CMD_SLEEP with '0', incdicating a wakeup
        paySetStatus(pld, status);
        paySetType(pld, CMD_SLEEP);
        radioSendPayload((WordVal) macGetDestAddr(), pld);
    }
}

// set up velocity profile structure  - assume 4 set points for now, generalize later
static void cmdSetVelProfile(unsigned char status, unsigned char length, unsigned char *frame) {
    Payload pld;
    PKT_UNPACK(_args_cmdSetVelProfile, argsPtr, frame);

    hallSetVelProfile(0, argsPtr->intervalsL, argsPtr->deltaL, argsPtr->velL);
    hallSetVelProfile(1, argsPtr->intervalsR, argsPtr->deltaR, argsPtr->velR);

    //Send confirmation packet
    pld = payCreateEmpty(sizeof(_args_cmdSetVelProfile));
    //pld->pld_data[0] = status;
    paySetStatus(pld, status);
    //pld->pld_data[1] = CMD_SET_VEL_PROFILE;
    paySetType(pld, CMD_SET_VEL_PROFILE);
    // packet length = 48 bytes (24 ints)
    memcpy((pld->pld_data) + 2, frame, sizeof(_args_cmdSetVelProfile));
    radioSendPayload((WordVal) macGetDestAddr(), pld);
}

// report motor position and  reset motor position (from Hall effect sensors)
// note motor_count is long (4 bytes)
void cmdZeroPos(unsigned char status, unsigned char length, unsigned char *frame) {

    radioSendPayload(macGetDestAddr(), payCreate(2*sizeof(long),
            (unsigned char *)(hallGetMotorCounts()), status, CMD_ZERO_POS));
    hallZeroPos(0);
    hallZeroPos(1);
}

// alternative telemetry which runs at 1 kHz rate inside PID loop
static void cmdHallTelemetry(unsigned char status, unsigned char length, unsigned char *frame) {
    //TODO: Integration of hall telemetry is unfinished. Fuction will currently
    // do nothing.

    //This is only commented to supress the warning
    //PKT_UNPACK(_args_cmdHallTelemetry, argsPtr, frame);
     
}

// send robot info when queried
void cmdWhoAmI(unsigned char status, unsigned char length, unsigned char *frame) {
    // maximum string length to avoid packet size limit
    char* verstr = versionGetString();
    int verlen = strlen(verstr);
    //The cast to unsigned char* is here to prevent a warning
    Payload pld = payCreate(verlen, (unsigned char*)verstr, status, CMD_WHO_AM_I);
    radioSendPayload(macGetDestAddr(), pld);
}

static void cmdSetHallGains(unsigned char status, unsigned char length, unsigned char *frame) {
    //Unpack unsigned char* frame into structured values
    PKT_UNPACK(_args_cmdSetPIDGains, argsPtr, frame);

    hallSetGains(0, argsPtr->Kp1, argsPtr->Ki1, argsPtr->Kd1, argsPtr->Kaw1, argsPtr->Kff1);
    hallSetGains(1, argsPtr->Kp2, argsPtr->Ki2, argsPtr->Kd2, argsPtr->Kaw2, argsPtr->Kff2);

    //Send confirmation packet
    Payload pld;
    pld = payCreateEmpty(20);
    //pld->pld_data[0] = status;
    //pld->pld_data[1] = CMD_SET_HALL_GAINS;
    paySetType(pld, CMD_SET_HALL_GAINS);
    paySetStatus(pld, status);
    memcpy((pld->pld_data) + 2, frame, 20);
    radioSendPayload((WordVal) macGetDestAddr(), pld);
}

static void cmdSetTailQueue(unsigned char status, unsigned char length, unsigned char *frame) {
    //The count is read via standard pointer math
    unsigned int count;
    count = (unsigned int) (*(frame));

    //Unpack unsigned char* frame into structured values
    //Here, unpacking happens in the loop below.
    //_args_cmdSetMoveQueue* argsPtr;
    int idx = sizeof (count); //should be an unsigned int, sizeof(uint) = 2

    tailCmdT tailSeg;
    int i;
    for (i = 0; i < count; i++) {
        tailSeg = (tailCmdT) malloc(sizeof (tailCmdStruct));
        //argsPtr = (_args_cmdSetMoveQueue*)(frame+idx);
        *tailSeg = *((tailCmdT) (frame + idx));
        tailqPush(tailq, tailSeg);
        //idx =+ sizeof(_args_cmdSetMoveQueue);
        idx += sizeof (tailCmdStruct);
    }
}

static void cmdSetTailGains(unsigned char status, unsigned char length, unsigned char *frame) {

    //Unpack unsigned char* frame into structured values
    //_args_cmdSetPIDGains* argsPtr = (_args_cmdSetPIDGains*) (frame);
    PKT_UNPACK(_args_cmdSetTailGains, argsPtr, frame);

    tailCtrlSetGains(argsPtr->Kp, argsPtr->Ki, argsPtr->Kd, argsPtr->Kaw, argsPtr->Kff);
	//tailCtrlSetGains(0,0,0,0,0);

    //Send confirmation packet
    Payload pld;
    pld = payCreateEmpty(sizeof(_args_cmdSetTailGains));
    pld->pld_data[0] = status;
    pld->pld_data[1] = CMD_SET_TAIL_GAINS;
    memcpy((pld->pld_data) + 2, frame, sizeof(_args_cmdSetTailGains));
    radioSendPayload(macGetDestAddr(), pld);
}


static void cmdSetThrustHall(unsigned char status, unsigned char length, unsigned char *frame) {
    PKT_UNPACK(_args_cmdSetThrustHall, argsPtr, frame);

    hallPIDSetInput(0 , argsPtr->chan1, argsPtr->runtime1);
    hallPIDOn(0);
    hallPIDSetInput(1 , argsPtr->chan1, argsPtr->runtime2);
    hallPIDOn(1);
}