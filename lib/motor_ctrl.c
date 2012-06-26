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

#define LEFT_TURN       _LATE2 = 1; _LATE4 = 0
#define RIGHT_TURN      _LATE2 = 0; _LATE4 = 1
#define NO_TURN         _LATE2 = 0; _LATE4 = 0

#define LED_LEFT_TURN       LED_GREEN = 1; LED_RED = 0
#define LED_RIGHT_TURN      LED_GREEN = 0; LED_RED = 1
#define LED_NO_TURN         LED_GREEN = 0; LED_RED = 0


static unsigned char steer_mode_ = MC_STEER_MODE_DISC;

static void mcSetupPeripheral(void);


void mcSetup(void) {

    mcSetupPeripheral();

}

void mcSetDutyCycle(unsigned char channel, float duty_cycle) {

    unsigned int pdc_value;
    pdc_value = (unsigned int)(2*duty_cycle/100*(10000));
    SetDCMCPWM(channel, pdc_value, 0);

}



void mcSteer(float value) {

    if (steer_mode_ == MC_STEER_MODE_CONT) {
        return;
    }

    if (value > 0) { 
        NO_TURN;
        RIGHT_TURN;
        LED_LEFT_TURN;
    } else if (value < 0) { 
        NO_TURN;
        LEFT_TURN;
        LED_RIGHT_TURN;
    } else { 
        LED_NO_TURN;
    }


}

static void mcSetupPeripheral(void) {

	unsigned int PTPERvalue = 2000;
    unsigned int SEVTCMPvalue, PTCONvalue, PWMCON1value, PWMCON2value;
	SEVTCMPvalue = 1988;
//    SEVTCMPvalue = 160; // Special Event Trigger Compare Value for ADC in phase with PWM
    //PTCONvalue = PWM_EN & PWM_IDLE_CON & PWM_OP_SCALE1 &
    //             PWM_IPCLK_SCALE4 & PWM_MOD_FREE;
	PTCONvalue = PWM_EN & PWM_IDLE_CON & PWM_OP_SCALE1 &
                 PWM_IPCLK_SCALE1 & PWM_MOD_FREE;
    PWMCON1value = PWM_MOD1_IND & PWM_PEN1L & PWM_MOD2_IND & PWM_PEN2L &
                   PWM_MOD3_IND & PWM_PEN3L & PWM_MOD4_IND & PWM_PEN4L;
    PWMCON2value = PWM_SEVOPS4 & PWM_OSYNC_TCY & PWM_UEN;
    ConfigIntMCPWM(PWM_INT_DIS & PWM_FLTA_DIS_INT & PWM_FLTB_DIS_INT);
    SetDCMCPWM(1, 0, 0);
    OpenMCPWM(PTPERvalue, SEVTCMPvalue, PTCONvalue, PWMCON1value, PWMCON2value);
	SetDCMCPWM(2, 0, 0);
    OpenMCPWM(PTPERvalue, SEVTCMPvalue, PTCONvalue, PWMCON1value, PWMCON2value);

	//Stan's
	
    // For 1KHz at MIPS == 40
    /*
	pwm_period_ = 624; 
    
    ConfigIntMCPWM(PWM_INT_DIS & PWM_FLTA_DIS_INT & PWM_FLTB_DIS_INT);

    PTPER = pwm_period_;

    PWMCON1bits.PMOD1 = 1;
    PWMCON1bits.PMOD2 = 1;
    PWMCON1bits.PMOD3 = 0;
    PWMCON1bits.PMOD4 = 0;
    PWMCON1bits.PEN1H = 0;
    PWMCON1bits.PEN2H = 0;
    PWMCON1bits.PEN3H = 0;
    PWMCON1bits.PEN4H = 0;
    PWMCON1bits.PEN1L = 1;
    PWMCON1bits.PEN2L = 1;
    PWMCON1bits.PEN3L = 0;
    PWMCON1bits.PEN4L = 0;

    PWMCON2bits.SEVOPS = 1; // postscale 1:1
    PWMCON2bits.OSYNC = 0;
    PWMCON2bits.IUE = 1;
	PWMCON2 |= PWM_UEN;
	
	P1SECMPbits.SEVTDIR = 1;

    PTCONbits.PTMOD = 0; // Free running mode
    PTCONbits.PTOPS = 0b11; // postscale 1:1
    PTCONbits.PTCKPS = 0b11; // postscale 1:64
    PTCONbits.PTSIDL = 0; // runs in CPU idle mode
    PTCONbits.PTEN = 1;
	
	PDC1 = 0x35;   // duty cycle = 0
	PDC2 = 0x35;
	SEVTCMP = 600; // Special Event Trigger Compare Value for ADC in phase with PWM
	//P1SECMPbits.SEVTCMP = 600;
	AD1CON1bits.SSRC = 0b011;
	*/
}



