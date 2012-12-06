#ifndef __STEERING_H
#define __STEERING_H

#include "pid.h"

void steeringSetup(void);
void steeringSetInput(int angRate);
void steeringSetGains(int Kp, int Ki, int Kd, int Kawm, int ff);
void steeringSetMode(unsigned int mode);
void steeringApplyCorrection(int* inputs, int* outputs);
void steeringOff();
void steeringOn();

//#define STEERING_SAT       1024
#define STEERING_SAT       4000 //for yaw-control steering

#ifdef PID_SOFTWARE
#define STEERING_KP  0
#define STEERING_KI  0
#define STEERING_KD  0
#define STEERING_KAW 0

#elif defined PID_HARDWARE
//Hardware PID
#define STEERING_KP  0
#define STEERING_KI  0
#define STEERING_KD  0
#define STEERING_KAW 0
#define STEERING_FF  0
#define STEERING_PID_ERR_SCALER 1

#endif

enum steerModeT{
    STEERMODE_OFF = 0,
    STEERMODE_INCREASE = 1,
    STEERMODE_DECREASE = 2,
    STEERMODE_SPLIT = 3,
    STEERMODE_YAW_DEC = 4,
    STEERMODE_YAW_SPLIT = 5
};

#endif //__STEERING_H
