//leg_ctrl.c

#include "pid.h"
#include "leg_ctrl.h"
#include "motor_ctrl.h"
#include "timer.h"
#include "adc_pid.h"
#include "pwm.h"
#include "led.h"
#include "adc.h"
#include "move_queue.h"
#include "math.h"
#include "steering.h"
#include<dsp.h>
#include <stdlib.h> // for malloc

#define ABS(my_val) ((my_val) < 0) ? -(my_val) : (my_val)

//PID container objects
pidObj motor_pidObjs[NUM_MOTOR_PIDS];
//DSP PID stuff
//These have to be declared here!
fractional motor_abcCoeffs[NUM_MOTOR_PIDS][3] __attribute__((section(".xbss, bss, xmemory")));
fractional motor_controlHists[NUM_MOTOR_PIDS][3] __attribute__((section(".ybss, bss, ymemory")));

//Timer 1 related variables
volatile unsigned long t1_ticks;
//Counter for blinking the red LED during motion
int blinkCtr;

//This is an option to force the PID outputs back to zero when there is no input.
//This was an attempt to stop bugs w/ motor twitching, or controller wandering.
//It may not be needed anymore.
#define PID_ZEROING_ENABLE 1

//Move queue variables, global
//TODO: move these into a move queue interface module
MoveQueue moveq;
moveCmdT currentMove, idleMove;
unsigned long currentMoveStart, moveExpire;

int bemf[NUM_MOTOR_PIDS]; //used to store the true, unfiltered speed
int bemfLast[NUM_MOTOR_PIDS]; // Last post-median-filter value
int bemfHist[NUM_MOTOR_PIDS][3]; //This is ONLY for applying the median filter to
int medianFilter3(int*);

int legCtrlOutputChannels[NUM_MOTOR_PIDS];

volatile char inMotion;

//Local scope functions
//TIMER1 driven
static void serviceMoveQueue(void);
static void moveSynth();
static void serviceMotionPID();
static void updateBEMF();

/*static struct piddata {
    int output[NUM_MOTOR_PIDS];
    unsigned int measurements[NUM_MOTOR_PIDS];
    long p[2], i[2], d[2];
} PIDTelemData;
 */


//////////////   Timer 1   ///////////////
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {

    serviceMoveQueue();
    moveSynth();

    serviceMotionPID();

    //Timer1 runs at 1kHz; t1_ticks is a ms counter
    t1_ticks++;

    //Clear Timer1 interrupt flag
    _T1IF = 0;
}

void SetupTimer1(void) {
    unsigned int T1CON1value, T1PERvalue;
    T1CON1value = T1_ON & T1_SOURCE_INT & T1_PS_1_1 & T1_GATE_OFF &
            T1_SYNC_EXT_OFF & T1_IDLE_CON;

    T1PERvalue = 0x9C40; //clock period = 0.001s = (T1PERvalue/FCY) (1KHz)
    //T1PERvalue = 0x9C40/2;
    t1_ticks = 0;
    OpenTimer1(T1CON1value, T1PERvalue);
    ConfigIntTimer1(T1_INT_PRIOR_6 & T1_INT_ON);
}

unsigned long getT1_ticks(){
    return t1_ticks;
}

/////////////////////////////////////////



void legCtrlSetup() {
    int i;
    motor_abcCoeffs[0][0] = 1;
    motor_abcCoeffs[0][1] = 2;
    motor_abcCoeffs[0][2] = 3;
    motor_abcCoeffs[1][0] = 1;
    motor_abcCoeffs[1][1] = 2;
    motor_abcCoeffs[2][2] = 3;
    for (i = 0; i < NUM_MOTOR_PIDS; i++) {
#ifdef PID_HARDWARE
        //THe user is REQUIRED to set up these pointers before initializing
        //the object, because the arrays are local to this module.
        motor_pidObjs[i].dspPID.abcCoefficients =
                motor_abcCoeffs[i];
        motor_pidObjs[i].dspPID.controlHistory =
                motor_controlHists[i];
#endif
        pidInitPIDObj(&(motor_pidObjs[i]), DEFAULT_KP, DEFAULT_KI, DEFAULT_KD, DEFAULT_KAW, DEFAULT_KFF);
        //Set up max's and saturation values
        motor_pidObjs[i].satValPos = SATTHROT;
        motor_pidObjs[i].satValNeg = 0;
        motor_pidObjs[i].maxVal = FULLTHROT;
        motor_pidObjs[i].minVal = 0;
    }

    //Set which PWM output each PID Object will correspond to
    legCtrlOutputChannels[0] = MC_CHANNEL_PWM1;
    legCtrlOutputChannels[1] = MC_CHANNEL_PWM2;

    SetupTimer1(); //this should be decoupled from the PID controllers
    //ADC_OffsetL = 1; //prevent divide by zero errors
    //ADC_OffsetR = 1;

    //Move Queue setup and initialization
    moveq = mqInit(8);
    idleMove = malloc(sizeof (moveCmdStruct));
    idleMove->inputL = 0;
    idleMove->inputR = 0;
    idleMove->duration = 0;
    idleMove->type = MOVE_SEG_IDLE;
    idleMove->params[0] = 0;
    idleMove->params[1] = 0;
    idleMove->params[2] = 0;
    currentMove = idleMove;

    currentMoveStart = 0;
    moveExpire = 0;
    blinkCtr = 0;
    inMotion = 0;

    //calibBatteryOffset(100);

    //Ensure controllers are reset to zero and turned off
    //External function used here since it will zero out the state
    pidSetInput(&(motor_pidObjs[0]), 0);
    pidSetInput(&(motor_pidObjs[1]), 0);
    motor_pidObjs[0].onoff = PID_OFF;
    motor_pidObjs[1].onoff = PID_OFF;

    //Set up filters and histories
    for (i = 0; i < NUM_MOTOR_PIDS; i++) {
        bemfLast[i] = 0;
        bemfHist[i][0] = 0;
        bemfHist[i][1] = 0;
        bemfHist[i][2] = 0;
    }
}

// Runs the PID controllers for the legs
void serviceMotionPID() {

    //Apply steering mixing, without overwriting anything
    int presteer[2] = {motor_pidObjs[0].input, motor_pidObjs[1].input};
    int poststeer[2] = {0, 0};
    steeringApplyCorrection(presteer, poststeer);
    motor_pidObjs[0].input = poststeer[0];
    motor_pidObjs[1].input = poststeer[1];

    updateBEMF();

    /////////// PID Section //////////
#ifdef PID_SOFTWARE
    int j;
    for (j = 0; j < NUM_MOTOR_PIDS; j++) {
        //We are now measuring battery voltage directly via AN0,
        // so the input offset to each PID loop can actually be tracked, and needs
        // to be updated. This should compensate for battery voltage drooping over time.
        motor_pidObjs[j].inputOffset = adcGetVBatt();

        //pidobjs[0] : Left side
        //pidobjs[0] : Right side
        if (motor_pidObjs[j].onoff) {
            //TODO: Do we want to add provisions to track error, even when
            //the output is switched off?

            //Update values
            pidUpdate(&(motor_pidObjs[j]), bemf[j]);

            //Set PWM duty cycle
            SetDCMCPWM(legCtrlOutputChannels[j], motor_pidObjs[j].output, 0);
        }//end of if (on / off)
        else if (PID_ZEROING_ENABLE) { //if PID loop is off
            SetDCMCPWM(legCtrlOutputChannels[j], 0, 0);
        }

    } // end of for(j)

#elif defined PID_HARDWARE
    int temp;
    int j;
    for (j = 0; j < NUM_MOTOR_PIDS; j++) {
        if (motor_pidObjs[j].onoff) {
            //pidSetReference(&PID1, pidObjs[0].input);
            //pidHWSetReference(&(motor_pidObjs[j].dspPID), 
            //        MOTOR_PID_ERR_SCALER * motor_pidObjs[j].input);
            temp = motor_pidObjs[j].input; //Save unscaled input val
            motor_pidObjs[j].input *= MOTOR_PID_SCALER; //Scale input
            pidUpdate(&(motor_pidObjs[j]),
                     MOTOR_PID_SCALER * bemf[j]); //Update with scaled feedback
            motor_pidObjs[j].input = temp;  //Reset unscaled input

            SetDCMCPWM(legCtrlOutputChannels[j], motor_pidObjs[j].output, 0);
        } else if (PID_ZEROING_ENABLE) { //Zero DC
            SetDCMCPWM(legCtrlOutputChannels[j], 0, 0);
        }

    } //end for(j)
#endif //PID_SOFTWWARE vs PID_HARDWARE
}



void updateBEMF(){
    //Back EMF measurements are made automatically by coordination of the ADC, PWM, and DMA.
    //Copy to local variables. Not strictly neccesary, just for clarity.
    //This **REQUIRES** that the divider on the battery & BEMF circuits have the same ratio.
    bemf[0] = adcGetVBatt() - adcGetBEMFL();
    bemf[1] = adcGetVBatt() - adcGetBEMFR();
    //NOTE: at this point, we should have a proper correspondance between
    //   the order of all the structured variable; bemf[i] associated with
    //   pidObjs[i], bemfLast[i], etc.
    //   Any "jumbling" of the inputs can be done in the above assignments.

    //Negative ADC measures mean nothing and should never happen anyway
    if (bemf[0] < 0) {
        bemf[0] = 0;
    }
    if (bemf[1] < 0) {
        bemf[1] = 0;
    }

    //Apply median filter
    int i;
    for (i = 0; i < NUM_MOTOR_PIDS; i++) {
        bemfHist[i][2] = bemfHist[i][1]; //rotate first
        bemfHist[i][1] = bemfHist[i][0];
        bemfHist[i][0] = bemf[i]; //include newest value
        bemf[i] = medianFilter3(bemfHist[i]); //Apply median filter
    }

    // IIR filter on BEMF: y[n] = 0.8 * y[n-1] + 0.2 * x[n]
    bemf[0] = (8 * (long) bemfLast[0] / 10) + 2 * (long) bemf[0] / 10;
    bemf[1] = (8 * (long) bemfLast[1] / 10) + 2 * (long) bemf[1] / 10;
    bemfLast[0] = bemf[0]; //bemfLast will not be used after here, OK to set
    bemfLast[1] = bemf[1];

    //Simple indicator if a leg is "in motion", via the yellow LED.
    //Not functionally necceasry; can be elimited to use the LED for something else.
    if ((bemf[0] > 0) || (bemf[1] > 0)) {
        LED_YELLOW = 1;
    } else {
        LED_YELLOW = 0;
    }
}



void serviceMoveQueue(void) {

    //Blink red LED when executing move program
    if (currentMove != idleMove) {
        if (blinkCtr == 0) {
            blinkCtr = 100;
            LED_RED = ~LED_RED;
        }
        blinkCtr--;
    }

    //Service Move Queue if not empty
    if (!mqIsEmpty(moveq)) {
        inMotion = 1;
        if ((currentMove == idleMove) || (t1_ticks >= moveExpire)) {
            currentMove = mqPop(moveq);
            moveExpire = t1_ticks + currentMove->duration;
            currentMoveStart = t1_ticks;
            steeringOn();

            //If we are no on an Idle move, turn on controllers
            if (currentMove->type != MOVE_SEG_IDLE) {
                motor_pidObjs[0].onoff = PID_ON;
                motor_pidObjs[1].onoff = PID_ON;
            }
        }
    }    //Move Queue is empty
    else if ((t1_ticks >= moveExpire) && currentMove != idleMove) {
        //No more moves, go back to idle
        currentMove = idleMove;
        pidSetInput(&(motor_pidObjs[0]), 0);
        motor_pidObjs[0].onoff = PID_OFF;
        pidSetInput(&(motor_pidObjs[1]), 0);
        motor_pidObjs[1].onoff = PID_OFF;
        moveExpire = 0;
        inMotion = 0; //for sleep, synthesis
        steeringOff();
    }
}

static void moveSynth() {
    //Move segment synthesis
    long ySL = currentMove->inputL; //store in local variable to limit lookups
    long ySR = currentMove->inputR; // "
    int yL = 0;
    int yR = 0;
    if (inMotion) {
        if (currentMove->type == MOVE_SEG_IDLE) {
            yL = 0;
            yR = 0;
        }
        if (currentMove->type == MOVE_SEG_CONSTANT) {
            yL = ySL;
            yR = ySR;
        }
        if (currentMove->type == MOVE_SEG_RAMP) {
            long rateL = (long) currentMove->params[0];
            long rateR = (long) currentMove->params[1];
            //Do division last to prevent integer math underflow
            yL = rateL * ((long) t1_ticks - (long) currentMoveStart) / 1000 + ySL;
            yR = rateR * ((long) t1_ticks - (long) currentMoveStart) / 1000 + ySR;
        }
        if (currentMove->type == MOVE_SEG_SIN) {
            //float temp = 1.0/1000.0;
            float amp = (float) currentMove->params[0];
            //float F = (float)currentMove->params[1] / 1000;
            float F = (float) currentMove->params[1] * 0.001;
#define BAMS16_TO_FLOAT 1/10430.367658761737
            float phase = BAMS16_TO_FLOAT * (float) currentMove->params[2]; //binary angle
            //Must be very careful about underflow & overflow here!
            //long arg = 2*BAMS16_PI*mF/100;
            //arg = arg*(t1_ticks - currentMoveStart)/10 - phase;
            //float fyL = amp*sin(2*3.1415*F*(float)(t1_ticks - currentMoveStart)/1000  - phase) + ySL;
            //float fyR = amp*sin(2*3.1415*F*(float)(t1_ticks - currentMoveStart)/1000  - phase) + ySR;
            float fyL = amp * sin(2 * 3.1415 * F * (float) (t1_ticks - currentMoveStart)*0.001 - phase) + ySL;
            float fyR = amp * sin(2 * 3.1415 * F * (float) (t1_ticks - currentMoveStart)*0.001 - phase) + ySR;


            //fractional arg = 2*(long)F*((long)t1_ticks-(long)currentMoveStart) *
            //fractional temp = _Q15sinPI(arg);
            //fractional wave = (int)((long)temp*(long)amp >> 15);

            //Clipping
            int temp = (int) fyL;
            if (temp < 0) {
                temp = 0;
            }
            yL = (unsigned int) temp;
            temp = (int) fyR;
            if (temp < 0) {
                temp = 0;
            }
            yR = (unsigned int) temp;
            //unsigned int yL = amp*sin(arg) + ySL;
        }
        motor_pidObjs[0].input = yL;
        motor_pidObjs[1].input = yR;
        if ((motor_pidObjs[0].input < 0) || (motor_pidObjs[1].input < 0)) {
            Nop();
            Nop();
        }
    }
    //Note hhere that pidObjs[n].input is not set if !inMotion, in case another behavior wants to
    // set it.
}



//Poor implementation of a median filter for a 3-array of values
int medianFilter3(int* a) {
    int b[3] = {a[0], a[1], a[2]};
    int temp;

    //Implemented through 3 compare-exchange operations, increasing index
    if (b[0] > b[1]) {
        temp = b[1];
        b[1] = b[0];
        b[0] = temp;
    }
    if (a[0] > a[2]) {
        temp = b[2];
        b[2] = b[0];
        b[0] = temp;
    }
    if (a[1] > a[2]) {
        temp = b[2];
        b[2] = b[1];
        b[1] = temp;
    }

    return b[1];
}

void legCtrlSetInput(unsigned int num, int val){
    pidSetInput(&(motor_pidObjs[num]), val);
}

void legCtrlOnOff(unsigned int num, unsigned char state){
    motor_pidObjs[num].onoff = state;
}

void legCtrlSetGains(unsigned int num, int Kp, int Ki, int Kd, int Kaw, int ff){
    pidSetGains(&(motor_pidObjs[num]), Kp, Ki, Kd, Kaw, ff);
}