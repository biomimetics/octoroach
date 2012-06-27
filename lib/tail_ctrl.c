//tail_ctrl.c

#include "tail_ctrl.h"
#include "pid.h"
//#include "motor_ctrl.h"
#include "timer.h"
#include "led.h"
#include "tail_queue.h"
#include "math.h"
#include "sys_service.h"
#include "move_queue.h"
#include <stdlib.h> // for malloc

#define ABS(my_val) ((my_val) < 0) ? -(my_val) : (my_val)

//PID container objects
pidObj tailPID;
//DSP PID stuff
//These have to be declared here!
fractional tail_abcCoeffs[3] __attribute__((section(".xbss, bss, xmemory")));
fractional tail_controlHists[3] __attribute__((section(".ybss, bss, ymemory")));

//Counter for blinking the red LED during motion
//int blinkCtr;

//This is an option to force the PID outputs back to zero when there is no input.
//This was an attempt to stop bugs w/ motor twitching, or controller wandering.
//It may not be needed anymore.
#define PID_ZEROING_ENABLE 1

//Tail queue
TailQueue tailq;
tailCmdT currentTail, idleTail;
unsigned long currentTailStart, tailExpire;

//Function to be installed into T1, and setup function
static void SetupTimer1(void);
static void tailCtrlServiceRoutine(void); //To be installed with sysService
//The following local functions are called by the service routine:
static void serviceTailQueue(void);
static void tailSynth();

volatile char tailInMotion;

/////////        Leg Control ISR       ////////
/////////  Installed to Timer1 @ 1Khz  ////////
//void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {

static void tailCtrlServiceRoutine(void) {
    serviceTailQueue(); //Update controllers
    tailSynth();
}

static void SetupTimer1(void) {
    unsigned int T1CON1value, T1PERvalue;
    T1CON1value = T1_ON & T1_SOURCE_INT & T1_PS_1_1 & T1_GATE_OFF &
            T1_SYNC_EXT_OFF & T1_IDLE_CON;

    T1PERvalue = 0x9C40; //clock period = 0.001s = (T1PERvalue/FCY) (1KHz)
    //T1PERvalue = 0x9C40/2;
    //getT1_ticks() = 0;
    //OpenTimer1(T1CON1value, T1PERvalue);
    //ConfigIntTimer1(T1_INT_PRIOR_6 & T1_INT_ON);
    int retval;
    retval = sysServiceConfigT1(T1CON1value, T1PERvalue, T1_INT_PRIOR_6 & T1_INT_ON);
    //TODO: Put a soft trap here, conditional on retval
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void tailCtrlSetup() {

    SetupTimer1(); // Timer 1 @ 1 Khz
    int retval;
    retval = sysServiceInstallT1(tailCtrlServiceRoutine);

    //Tail queue
    tailq = tailqInit(16);
    idleTail = malloc(sizeof (tailCmdStruct));
    idleTail->torque = 0.0;
    idleTail->duration = 0;
    idleTail->type = TAIL_SEG_IDLE;
    idleTail->params[0] = 0;
    idleTail->params[1] = 0;
    idleTail->params[2] = 0;
    currentTail = idleTail;
    currentTailStart = 0;
    tailExpire = 0;

 #ifdef PID_HARDWARE
    //Create PID controller object
    tailPID.dspPID.abcCoefficients = tail_abcCoeffs;
    tailPID.dspPID.controlHistory = tail_controlHists;
#endif
    pidInitPIDObj(&tailPID, TAIL_DEFAULT_KP, TAIL_DEFAULT_KI,
            TAIL_DEFAULT_KD, TAIL_DEFAULT_KAW, 0);
    tailPID.satValPos = 100;
    tailPID.satValNeg = -100;
    tailPID.maxVal = 100;
    tailPID.minVal = -100;

    tailPID.onoff = PID_OFF;

}


////// Tail functions below here

static void serviceTailQueue(void) {
    //Service Move Queue if not empty
    if (!tailqIsEmpty(tailq)) {
        tailInMotion = 1;
        if ((currentTail == idleTail) || (getT1_ticks() >= tailExpire)) {
            currentTail = tailqPop(tailq);
            tailExpire = getT1_ticks() + currentTail->duration;
            currentTailStart = getT1_ticks();

            //If we are no on an Idle move, turn on controllers
            if (currentTail->type != TAIL_SEG_IDLE) {
                //TODO: Turn on tail controller
            }
        }
    }//Move Queue is empty
    else if ((getT1_ticks() >= tailExpire) && currentTail != idleTail) {
        //No more moves, go back to idle
        currentTail = idleTail;
        //TODO: Zero tail torque, turn off controller
        tailExpire = 0;
    }
}

static void tailSynth() {
    //Move segment synthesis
    long yS = currentTail->torque; //store in local variable to limit lookups
    int y = 0;
    if (tailInMotion) {
        if (currentTail->type == TAIL_SEG_IDLE) {
            y = 0;
        }
        if (currentTail->type == TAIL_SEG_CONSTANT) {
            y = yS;
        }
        if (currentTail->type == TAIL_SEG_RAMP) {
            long rate = (long) currentTail->params[0];
            //Do division last to prevent integer math underflow
            y = rate * ((long) getT1_ticks() - (long) currentTailStart) / 1000 + yS;
        }
        if (currentTail->type == TAIL_SEG_SIN) {
            //float temp = 1.0/1000.0;
            float amp = (float) currentTail->params[0];
            //float F = (float)currentMove->params[1] / 1000;
            float F = (float) currentTail->params[1] * 0.001;
#define BAMS16_TO_FLOAT 1/10430.367658761737
            float phase = BAMS16_TO_FLOAT * (float) currentTail->params[2]; //binary angle
            float fy = amp * sin(2 * 3.1415 * F * (float) (getT1_ticks() -
                    currentTailStart)*0.001 - phase) + yS;

            //Clipping
            int temp = (int) fy;
            if (temp < 0) {
                temp = 0;
            }
            y = (unsigned int) temp;
        }
        //TODO: Set tail input here
        //motor_pidObjs[0].input = yL;
    }
    //Note here that pidObjs[n].input is not set if !inMotion, in case another behavior wants to
    // set it.
}

void tailCtrlSetGains(int Kp, int Ki, int Kd, int Kaw, int ff) {
    pidSetGains(&tailPID, Kp, Ki, Kd, Kaw, ff);
}

void tailCtrlOnOff(unsigned char state) {
    tailPID.onoff = state;
}

void tailCtrlSetInput(int val){
    pidSetInput(&tailPID, val);
}
