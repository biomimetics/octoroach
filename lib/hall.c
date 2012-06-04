// July 14, 2011
// R. Fearing
// code for position feedback using hall sensors on left and right motor

#include "p33Fxxxx.h"
#include "timer.h"
#include "incap.h"
#include "led.h"
#include "stopwatch.h"

int t2_ticks;     // keep track of timer 2 overflow
long old_right_time, right_time, right_delta; // time of last event
// unsigned long tic, toc;
long old_left_time, left_time, left_delta;
long motor_count[2]; // 0 = left 1 = right counts on sensor

//Hall effect sensor has ~ 3 kHz rate max, so choose clock high enough
// choose clock period = 6.4 us, divide FCY by 256

void SetupTimer2(void)
{
    unsigned int T2CON1value, T2PERvalue;
   t2_ticks = 0;
    T2CON1value = T2_ON & T2_SOURCE_INT & T2_PS_1_256 & T2_GATE_OFF;
    T2PERvalue = 0xffff; // max period 
    OpenTimer2(T2CON1value, T2PERvalue);    
// enable interrupts, lower priority
    ConfigIntTimer2(T2_INT_ON & T2_INT_PRIOR_6);
    EnableIntT2;
}

// Input Capture 7: left legs
// Input Capture 8: right legs

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void) {

t2_ticks++; // updates about every 400 ms

    //Clear Timer2 interrupt flag
    _T2IF = 0;
}


void SetupInputCapture()
{ 
 // RB4 and RB5 will be used for inputs
_TRISB4 = 1;  // set for input
_TRISB5 = 1; // set for input

//right leg
motor_count[0] = 0;
old_right_time = 0;
// choose low priority, since hardware cpatures time anyway
ConfigIntCapture8(IC_INT_ON & IC_INT_PRIOR_2);
    EnableIntIC8;
/* In Edge Detection Mode (ICM = 001), the interrupt is generated on every capture
event and the Input Capture Interrupt (ICI<1:0>) bits are ignored. */
// every edge to double resolution

    OpenCapture8(IC_IDLE_STOP & IC_TIMER2_SRC &  
                 IC_INT_1CAPTURE & IC_EVERY_EDGE);
// left leg
motor_count[1] = 0;
old_left_time = 0;
ConfigIntCapture7(IC_INT_ON & IC_INT_PRIOR_2);
    EnableIntIC7;
/* In Edge Detection Mode (ICM = 001), the interrupt is generated on every capture
event and the Input Capture Interrupt (ICI<1:0>) bits are ignored. */
    OpenCapture7(IC_IDLE_STOP & IC_TIMER2_SRC &  
                 IC_INT_1CAPTURE & IC_EVERY_EDGE);
}

unsigned long toc;
// handler for right leg
void __attribute__ ((__interrupt__, no_auto_psv)) _IC8Interrupt(void)
{
//  toc = swatchToc(); // elapsed time since last rising edge
// Insert ISR code here
    motor_count[0]++;  // increment count for right side - neglect overflow/wrap around
    right_time = (long) IC8BUF + ((long)(t2_ticks) << 16);
    right_delta = right_time- old_right_time;
    old_right_time = right_time;

    LED_RED = ~LED_RED;
    IFS1bits.IC8IF = 0; // Clear CN interrupt
//  tic = swatchTic();
}

//handler for left leg
void __attribute__ ((__interrupt__, no_auto_psv)) _IC7Interrupt(void)
{
// Insert ISR code here
    motor_count[1]++; // increment count for right side - neglect overflow/wrap around

    left_time = (long) IC7BUF + ((long)(t2_ticks) << 16);
    left_delta = left_time- old_left_time;
    old_left_time = left_time;

    LED_GREEN = ~LED_GREEN;
  
    IFS1bits.IC7IF = 0; // Clear CN interrupt
}

