// Contents of this file are copyright Andrew Pullin, 2013

#ifndef __LEG_CTRL_H
#define __LEG_CTRL_H

#define HALFTHROT 2000
#define FULLTHROT 2*HALFTHROT
//#define MAXTHROT 3976
#define SATTHROT (int)((3976.0/4000.0)*(float)FULLTHROT)

#define NUM_MOTOR_PIDS 2

// These defines will make a correspondance for other modules calling the
// functions below that take in a "num" for a given motor.
//motor_pidObjs[0] --> Left Legs
//motor_pidObjs[1] --> Right legs
#define LEG_CTRL_LEFT  0
#define LEG_CTRL_RIGHT 1

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

void legCtrlSetup();
void legCtrlSetInput(unsigned int num, int val);
void legCtrlOnOff(unsigned int num, unsigned char state);
void legCtrlSetGains(unsigned int num, int Kp, int Ki, int Kd, int Kaw, int ff);

#endif
