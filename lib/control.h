/*********************************
* Name: control.h
* Desc: Header for Control Law
* Date: 2009-10-30
* Author: stanbaek
*********************************/

#ifndef __CTRL_H
#define __CTRL_H


void ctrlSetup(void);
void ctrlSetAutoControl(unsigned char flag, float ref, float offset, float Kp, float Ki, float Kd);
void ctrlTxDutyCycle(unsigned long count);
#endif  // __CTRL_H


