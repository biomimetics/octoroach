#ifndef __STEERING_H
#define __STEERING_H

void steeringSetup(void);
void setSteeringAngRate(int angRate);
void UpdatePIDSteering(pidT *pid, int y);
void steeringSetGains(int Kp,int Ki,int Kd,int Kawm, int ff);
void steeringSetMode(unsigned int mode);
void steeringHandleISR();
void steeringApplyCorrection(int* inputs, int* outputs);
void steeringOff();
void steeringOn();

#define SATVAL       5000
//#define STEERING_KP  25
//#define STEERING_KI  2
//#define STEERING_KD  0
//#define STEERING_KAW 2
#define STEERING_KP  0
#define STEERING_KI  0
#define STEERING_KD  0
#define STEERING_KAW 0
#define STEERING_GAIN_SCALER 512

enum STEERING_MODES { 
	STEERMODE_INCREASE = 1,
	STEERMODE_DECREASE = 0,
	STEERMODE_SPLIT	   = 2
};

#endif //__STEERING_H
