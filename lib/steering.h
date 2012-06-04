#ifndef __STEERING_H
#define __STEERING_H

#include "pid.h"

void steeringSetup(void);
void steeringSetAngRate(int angRate);
void steeringSetGains(int Kp,int Ki,int Kd,int Kawm, int ff);
void steeringSetMode(unsigned int mode);
void steeringHandleISR();
void steeringApplyCorrection(int* inputs, int* outputs);
void steeringOff();
void steeringOn();

#define STEERING_SAT       5000
#define STEERING_GAIN_SCALER 512 //Software PID specific; TODO

//Select DSP core PID
#define PID_HARDWARE

#ifdef PID_SOFTWARE
#define STEERING_KP  0
#define STEERING_KI  0
#define STEERING_KD  0
#define STEERING_KAW 0

#elif defined PID_HARDWARE
//Hardware PID
#define STEERING_KP  10
#define STEERING_KI  1
#define STEERING_KD  0
#define STEERING_KAW 0
#define STEERING_FF  0
#define STEERING_PID_ERR_SCALER 1

#endif

enum STEERING_MODES { 
	STEERMODE_INCREASE = 1,
	STEERMODE_DECREASE = 0,
	STEERMODE_SPLIT	   = 2
};

#endif //__STEERING_H
