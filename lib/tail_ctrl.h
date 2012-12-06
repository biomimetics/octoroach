#ifndef __TAIL_CTRL_H
#define __TAIL_CTRL_H

//A controller is not being used yet

//Default gains
#ifdef PID_SOFTWARE
#define TAIL_DEFAULT_KP  200
#define TAIL_DEFAULT_KI  5
#define TAIL_DEFAULT_KD  0
#define TAIL_DEFAULT_KAW 5
#define TAIL_DEFAULT_KFF  0
#define SOFT_GAIN_SCALER 512
#elif defined PID_HARDWARE
#define TAIL_DEFAULT_KP  500
#define TAIL_DEFAULT_KI  50
#define TAIL_DEFAULT_KD  0
#define TAIL_DEFAULT_KAW 0
#define TAIL_DEFAULT_KFF  0
#define TAIL_PID_SCALER 8
#endif


void tailCtrlSetup();
void tailCtrlSetGains(int Kp, int Ki, int Kd, int Kaw, int ff);
void tailCtrlOnOff(unsigned char state);
void tailCtrlSetInput(int val);

#endif   // __TAIL_CTRL_H
