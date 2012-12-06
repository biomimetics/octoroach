/*********************************************
 * Name: motor_ctrl.c
 * Desc: Motor Controller (PWM)
 * Date: 2010-05-30
 * Author: stanbaek
 *********************************************/

#include "motor_ctrl.h"
#include "pwm.h"
#include "ports.h"
#include "led.h"


#define LEFT_TURN           _LATE2 = 1; _LATE4 = 0
#define RIGHT_TURN          _LATE2 = 0; _LATE4 = 1
#define NO_TURN             _LATE2 = 0; _LATE4 = 0

#define LEFT_TURN_CHANNEL   2
#define RIGHT_TURN_CHANNEL  3

#define LED_LEFT_TURN       LED_GREEN = 1; LED_RED = 0
#define LED_RIGHT_TURN      LED_GREEN = 0; LED_RED = 1
#define LED_NO_TURN         LED_GREEN = 0; LED_RED = 0


static unsigned int pwmPeriod;
static unsigned char steerMode = MC_STEER_MODE_CONT;

static void mcSetupPeripheral(void);

void mcSetSteerMode(unsigned char mode);

void mcSetup(void) {

    mcSetupPeripheral();
    //H-bridge for driving tail motor
    mcSetSteerMode(MC_STEER_MODE_CONT);

}

void mcSetDutyCycle(unsigned char channel, float duty_cycle) {

    unsigned int pdc_value;
    pdc_value = (unsigned int) (2 * duty_cycle / 100 * pwmPeriod);
    SetDCMCPWM(channel, pdc_value, 0);

}

void mcThrust(float value) {

    mcSetDutyCycle(MC_CHANNEL_PWM1, value);

}

void mcSteer(float value) {

    //char lcdstr[80];

    if (steerMode == MC_STEER_MODE_CONT) {
        if (value > 0) {
            mcSetDutyCycle(RIGHT_TURN_CHANNEL, 0);
            mcSetDutyCycle(LEFT_TURN_CHANNEL, value);
        } else if (value < 0) {
            mcSetDutyCycle(LEFT_TURN_CHANNEL, 0);
            mcSetDutyCycle(RIGHT_TURN_CHANNEL, -value);
        } else {
            PDC2 = 0;
            PDC3 = 0;
        }

    } else {
        if (value > 0) {
            NO_TURN;
            RIGHT_TURN;
        } else if (value < 0) {
            NO_TURN;
            LEFT_TURN;
        } else {
            NO_TURN;
        }
    }

}

void mcSetSteerMode(unsigned char mode) {

    if (mode == MC_STEER_MODE_DISC) {
        steerMode = MC_STEER_MODE_DISC;
        // PWM2L & PWM3L pins are general I/O
        PWMCON1bits.PEN2L = 0;
        PWMCON1bits.PEN3L = 0;
    } else {
        steerMode = MC_STEER_MODE_CONT;
        // PWM2L & PWM3L pins are enabled for PWM output
        PWMCON1bits.PEN2L = 1;
        PWMCON1bits.PEN3L = 1;
    }

}

static void mcSetupPeripheral(void) {

    //////////// NKOHUT ///////////
    /*
    unsigned int PTPERvalue = 8000;
    unsigned int SEVTCMPvalue, PTCONvalue, PWMCON1value, PWMCON2value;
    SEVTCMPvalue = 7952;
    PTCONvalue = PWM_EN & PWM_IDLE_CON & PWM_OP_SCALE1 &
            PWM_IPCLK_SCALE1 & PWM_MOD_FREE;
    PWMCON1value = PWM_MOD1_IND & PWM_PEN1L & PWM_MOD2_IND & PWM_PEN2L &
            PWM_MOD3_IND & PWM_PEN3L & PWM_MOD4_IND & PWM_PEN4L;
    PWMCON2value = PWM_SEVOPS4 & PWM_OSYNC_TCY & PWM_UEN;
    ConfigIntMCPWM(PWM_INT_DIS & PWM_FLTA_DIS_INT & PWM_FLTB_DIS_INT);
    OpenMCPWM(PTPERvalue, SEVTCMPvalue, PTCONvalue, PWMCON1value, PWMCON2value);
    SetDCMCPWM(1, 0, 0);
    SetDCMCPWM(2, 0, 0);
    SetDCMCPWM(3, 0, 0);
    SetDCMCPWM(4, 0, 0);

    pwmPeriod = PTPERvalue;
    */

    //////////// APULLIN ///////////
    
     unsigned int PTPERvalue = 2000;
    unsigned int SEVTCMPvalue, PTCONvalue, PWMCON1value, PWMCON2value;
    SEVTCMPvalue = 1988;
    //    SEVTCMPvalue = 160; // Special Event Trigger Compare Value for ADC in phase with PWM
    //PTCONvalue = PWM_EN & PWM_IDLE_CON & PWM_OP_SCALE1 &
    //             PWM_IPCLK_SCALE4 & PWM_MOD_FREE;
    PTCONvalue = PWM_EN & PWM_IDLE_CON & PWM_OP_SCALE4 &
            PWM_IPCLK_SCALE1 & PWM_MOD_FREE;
    PWMCON1value = PWM_MOD1_IND & PWM_PEN1L & PWM_MOD2_IND & PWM_PEN2L &
            PWM_MOD3_IND & PWM_PEN3L & PWM_MOD4_IND & PWM_PEN4L;
    PWMCON2value = PWM_SEVOPS4 & PWM_OSYNC_TCY & PWM_UEN;
    ConfigIntMCPWM(PWM_INT_DIS & PWM_FLTA_DIS_INT & PWM_FLTB_DIS_INT);
    OpenMCPWM(PTPERvalue, SEVTCMPvalue, PTCONvalue, PWMCON1value, PWMCON2value);
    SetDCMCPWM(1, 0, 0);
    SetDCMCPWM(2, 0, 0);
     
}
