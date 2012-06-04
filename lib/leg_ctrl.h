#ifndef __LEG_CTRL_H
#define __LEG_CTRL_H

#define HALFTHROT 2000
#define FULLTHROT 2*HALFTHROT
//#define MAXTHROT 3976
#define SATTHROT (int)((3976.0/4000.0)*(float)FULLTHROT)

#define NUM_MOTOR_PIDS 2

void legCtrlSetup();
void SetupTimer1(void);
unsigned long getT1_ticks();
void legCtrlSetInput(unsigned int num, int val);
void legCtrlOnOff(unsigned int num, unsigned char state);
void legCtrlSetGains(unsigned int num, int Kp, int Ki, int Kd, int Kaw, int ff);

#endif
