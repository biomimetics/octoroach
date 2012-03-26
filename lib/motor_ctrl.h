/*********************************************
* Name: motor_ctrl.h
* Desc: Motor Controller (PWM)
* Date: 2010-05-30
* Author: stanbaek
*********************************************/

#ifndef __MOTOR_CTRL_H
#define __MOTOR_CTRL_H


#define MC_CHANNEL_PWM1     1
#define MC_CHANNEL_PWM2     2
#define MC_CHANNEL_PWM3     3
#define MC_CHANNEL_PWM4     4

#define MC_STEER_MODE_DISC  0   
#define MC_STEER_MODE_CONT  1   

#define MC_STEER_LEFT       -1.0
#define MC_STEER_RIGHT      1.0
#define MC_STEER_STRAIGHT   0.0



// by default, all RE ports are output
void mcSetup(void);


// the resolution of the duty cycle is 1/(2*PTPER)
void mcSetDutyCycle(unsigned char channel, float duty_cycle);

// -100 <= value <= 100 for continuous mode
// value >0, =0, <0 for discrete mode (default mode)
void mcSteer(float value);

#endif  // __MOTOR_CTRL_H
