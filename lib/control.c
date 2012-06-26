/*
 * Copyright (c) 2009 - 2010, Regents of the University of California
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the University of California, Berkeley nor the names
 *   of its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * Control Block
 *
 * by Stanley S. Baek
 *
 * v.beta
 *
 * Revisions:
 *  Stanley S. Baek      2009-10-30    Initial release
 *                      
 * 
 */


#include "control.h"
#include "timer.h"
#include "utils.h"
#include "sensors.h"
#include "payload.h"
#include "radio.h"
#include "motor_ctrl.h"
#include "led.h"
#include "cmd_const.h"
#include "stopwatch.h"

/*-----------------------------------------------------------------------------
 *          Static Variables
-----------------------------------------------------------------------------*/
struct {
    volatile unsigned char is_running;
    float ref;
    float offset;
    float Kp;
    float Ki;
    float Kd;
} PitchCtrlParams;

static volatile unsigned long tx_count_ = 0;

/*-----------------------------------------------------------------------------
 *      Declaration of static functions 
-----------------------------------------------------------------------------*/
static void ctrlRegulatePitch(void);


/*-----------------------------------------------------------------------------
 *          Public functions
-----------------------------------------------------------------------------*/

void ctrlSetup(void) {

    // period value = Fcy/(prescale*Ftimer)
    unsigned int con_reg, period;

    // prescale 1:64
    con_reg = T2_ON & T2_IDLE_STOP & T2_GATE_OFF & T2_PS_1_64 & T2_32BIT_MODE_OFF & T2_SOURCE_INT;
    //Period is set so that period = 0.01s (100Hz), MIPS = 40
    period = 6250;

    OpenTimer2(con_reg, period);

    // interrupt for control law
    ConfigIntTimer2(T2_INT_PRIOR_6 & T2_INT_ON);
    // ConfigIntTimer2(T2_INT_PRIOR_6 & T2_INT_OFF);

}


void ctrlSetAutoControl(unsigned char flag, float ref, float offset, float Kp, float Ki, float Kd) {
    
    PitchCtrlParams.is_running = flag;
    PitchCtrlParams.ref = ref;
    PitchCtrlParams.offset = offset;
    PitchCtrlParams.Kp = Kp;
    PitchCtrlParams.Ki = Ki;
    PitchCtrlParams.Kd = Kd;

    if (flag == 0) {
        mcSetDutyCycle(MC_CHANNEL_PWM1, 0.0);
    }

}

void ctrlTxDutyCycle(unsigned long count) {

    tx_count_ = count;

}


/*-----------------------------------------------------------------------------
 * ----------------------------------------------------------------------------
 * The functions below are intended for internal use, i.e., private methods.
 * Users are recommended to use functions defined above.
 * ----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

/*****************************************************************************
* Function Name : _T2Interrupt
* Description   : Interrupt handler for Timer 2
* Parameters    : None
* Return Value  : None
*****************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void) {

    if (PitchCtrlParams.is_running) {
        ctrlRegulatePitch();
    }

    _T2IF = 0;

}



static void ctrlRegulatePitch(void) {

    union {
        float fval;
        unsigned char cval[4];
    } DutyCycle;

    union {
        unsigned long lval;
        unsigned char cval[4];
    } Time;

    static int count = 0;
    float pitch = senGetPitch(); 
    Payload pld;
    
    DutyCycle.fval = PitchCtrlParams.offset + PitchCtrlParams.Kp*(PitchCtrlParams.ref - pitch);

    mcSetDutyCycle(MC_CHANNEL_PWM1, DutyCycle.fval);


    Time.lval = swatchTic();

    if (tx_count_ > 0) {
        pld = payCreateEmpty(10);
        paySetType(pld, CMD_TX_DUTY_CYCLE);
        paySetStatus(pld, 0);
        payAppendData(pld, 0, 4, Time.cval);
        payAppendData(pld, 4, 4, DutyCycle.cval);
        radioSendPayload(pld);
        tx_count_--;

        if (count == 20) {
            LED_GREEN = ~LED_GREEN;
            count = 0;
        }
        count++;
    } else {
        LED_GREEN = 0;
    }


}





