#ifndef __TAIL_CTRL_H
#define __TAIL_CTRL_H

//A controller is not being used yet
/*
//Default gains
#ifdef PID_SOFTWARE
#define LEG_DEFAULT_KP  200
#define LEG_DEFAULT_KI  5
#define LEG_DEFAULT_KD  0
#define LEG_DEFAULT_KAW 5
#define LEG_DEFAULT_KFF  0
#define SOFT_GAIN_SCALER 512
#elif defined PID_HARDWARE
#define LEG_DEFAULT_KP  15000
#define LEG_DEFAULT_KI  500
#define LEG_DEFAULT_KD  150
#define LEG_DEFAULT_KAW 0
#define LEG_DEFAULT_KFF  0
#define MOTOR_PID_SCALER 32
#endif
*/

void tailCtrlSetup();
//void legCtrlSetInput(unsigned int num, int val);
//void legCtrlOnOff(unsigned int num, unsigned char state);
//void legCtrlSetGains(unsigned int num, int Kp, int Ki, int Kd, int Kaw, int ff);

#endif   // __TAIL_CTRL_H
