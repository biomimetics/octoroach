#include "settings.h"

#include "pid.h"
#include "timer.h"
#include "gyro.h"
#include "steering.h"
#include "dfmem.h"
#include "telem.h"
#include "move_queue.h"
#include "xl.h"
#include "pid_hw.h"
#include "leg_ctrl.h"
#include "sys_service.h"
#include "imu.h"

//Inline functions
#define ABS(a)	   (((a) < 0) ? -(a) : (a))

//float lastGyroValue = 0.0;

//Steering controller variables
pidObj steeringPID;

fractional steering_abcCoeffs[3] __attribute__((section(".xbss, bss, xmemory")));
fractional steering_controlHists[3] __attribute__((section(".ybss, bss, ymemory")));

//#define GYRO_AVG_SAMPLES 	32

static unsigned int steeringMode;

float steeringInitialYaw;

extern moveCmdT currentMove, idleMove;
extern char inMotion;

//Function to be installed into T5, and setup function
static void SetupTimer5();
static void steeringServiceRoutine(void);  //To be installed with sysService
//The following local functions are called by the service routine:
static void steeringHandleISR();


////   Private functions
////////////////////////

/////////        Steering ISR          ////////
////////  Installed to Timer5 @ 300hz  ////////
//void __attribute__((interrupt, no_auto_psv)) _T5Interrupt(void) {
static void steeringServiceRoutine(void){
    //This intermediate function is used in case we want to tie other
    //sub-tasks to the steering service routine.
    //TODO: Is this neccesary?

    // Steering update ISR handler
    steeringHandleISR();
}

static void SetupTimer5(){
    ///// Timer 5 setup, Steering ISR, 300Hz /////
    // period value = Fcy/(prescale*Ftimer)
    unsigned int T5CON1value, T5PERvalue;
    // prescale 1:64
    T5CON1value = T5_ON & T5_IDLE_CON & T5_GATE_OFF & T5_PS_1_64 & T5_SOURCE_INT;
    // Period is set so that period = 5ms (200Hz), MIPS = 40
    //period = 3125; // 200Hz
    T5PERvalue = 2083; // ~300Hz
    int retval;
    retval = sysServiceConfigT5(T5CON1value, T5PERvalue, T5_INT_PRIOR_5 & T5_INT_ON); 
}



////   Public functions
////////////////////////

void steeringSetup(void) {

#ifdef PID_HARDWARE
    //Create PID controller object
    steeringPID.dspPID.abcCoefficients = steering_abcCoeffs;
    steeringPID.dspPID.controlHistory = steering_controlHists;
#endif
    pidInitPIDObj(&steeringPID, STEERING_KP, STEERING_KI, STEERING_KD, STEERING_KAW, 0);
    steeringPID.satValPos = STEERING_SAT;
    steeringPID.satValNeg = -STEERING_SAT;
    steeringPID.maxVal = STEERING_SAT;
    steeringPID.minVal = -STEERING_SAT;

    steeringSetInput(0);

    SetupTimer5(); //T5 ISR will update the steering controller
    int retval;
    retval = sysServiceInstallT5(steeringServiceRoutine);

    //Averaging filter setup:
    //filterAvgCreate(&gyroZavg, GYRO_AVG_SAMPLES);
    //This is now owned by imu.c

    steeringPID.onoff = PID_ON; //OFF by default

    steeringMode = STEERMODE_DECREASE;
}

void steeringSetInput(int steerInput) {
    steeringPID.input = steerInput;
}

void steeringSetGains(int Kp, int Ki, int Kd, int Kaw, int ff) {
    pidSetGains(&steeringPID, Kp, Ki, Kd, Kaw, ff);
}

void steeringSetMode(unsigned int sm) {
    steeringMode = sm;
    //TODO: this could probably be put in a better place
    if(steeringMode == STEERMODE_OFF){
        steeringPID.onoff = PID_OFF;
    }
    else{ 
        steeringPID.onoff = PID_ON;
    }



    //Only for RELATIVE turns
    //if ((steeringMode == STEERMODE_YAW_DEC) || (steeringMode == STEERMODE_YAW_DEC))) {
    //    steeringInitialYaw = imuGetBodyZPositionDeg();
    //}
}

static void steeringHandleISR() {

    int steeringFeedback;  //Could be yaw OR yaw rate
    float relativeYaw = 0.0;

    if((steeringMode == STEERMODE_YAW_DEC) || (steeringMode == STEERMODE_YAW_SPLIT)){
        relativeYaw = imuGetBodyZPositionDeg() - steeringInitialYaw;
        steeringFeedback = (int)(14.375*relativeYaw);
    }
    else{
        steeringFeedback = imuGetGyroZValueAvg();
    }
    //Threshold filter on gyro to account for minor drift
    //if (ABS(wz) < GYRO_DRIFT_THRESH) {
    //    wz = 0;
    //}

    //Update the setpoints
    //if((currentMove->inputL != 0) && (currentMove->inputR != 0)){
    if ((currentMove != idleMove) || (inMotion == 1) ) {
        //Only update steering controller if we are in motion
#ifdef PID_SOFTWARE
        pidUpdate(&steeringPID, steeringFeedback);
#elif defined PID_HARDWARE
        int temp = 0;
        temp = steeringPID.input; //Save unscaled input val
        steeringPID.input *= STEERING_PID_ERR_SCALER; //Scale input
        pidUpdate(&steeringPID,
                 STEERING_PID_ERR_SCALER * steeringFeedback); //Update with scaled feedback
       steeringPID.input = temp;  //Reset unscaled input
#endif   //PID_SOFTWWARE vs PID_HARDWARE
    }
    //Now the output correction is stored in steeringPID.output,
    //which will be read later when the steering mixing is done.
}

void steeringApplyCorrection(int* inputs, int* outputs) {
    int left, right;
    //Steering Mixing
    //left = currentMove->inputL;
    //right = currentMove->inputR;
    left = inputs[0];
    right = inputs[1];

    if (steeringPID.onoff == PID_ON) {
        int delta = steeringPID.output;
        
        if ((steeringMode == STEERMODE_DECREASE) || (steeringMode == STEERMODE_YAW_DEC)) {
            // Depending on which way the bot is turning, choose which side to add correction to
            if (steeringPID.output <= 0) {
                //right = right + steeringPID.output;
                right = right + delta;
                if (right < 0) {
                    right = 0;
                } //clip right channel to zero
            } else //if(steeringPID.output > 0)
            {
                left = left - delta;
                //left = left - steeringPID.output;
                if (left < 0) {
                    left = 0;
                } //clip right channel to zero (one, actually)
            }
        } else if (steeringMode == STEERMODE_INCREASE) {
            // Depending on which way the bot is turning, choose which side to add correction to
            if (steeringPID.output <= 0) {
                //right = right + steeringPID.output;
                left = left - delta;
                if (right < 0) {
                    right = 0;
                } //clip right channel to zero
            } else //if(steeringPID.output > 0)
            {
                right = right + delta;
                //left = left - steeringPID.output;
                if (left < 0) {
                    left = 0;
                } //clip right channel to zero
            }
        } else if ((steeringMode == STEERMODE_SPLIT) || (steeringMode == STEERMODE_YAW_SPLIT)) {
            right = right + delta / 2;
            left = left - delta / 2;
            if (right < 0) {
                left = left - right; //increase left, since right < 0
                right = 0;
            } //clip right channel to zero
            if (left < 0) {
                right = right - left; //increase right, since left < 0
                left = 0;
            } //clip left channel to zero
        } else if (steeringMode == STEERMODE_OFF)  {
            //Do nothing, pass left and right unchanged
        }
    }//endif steeringPID.onoff

    outputs[0] = left;
    outputs[1] = right;

}

void steeringOff() {
    steeringPID.onoff = PID_OFF;
    steeringPID.p = 0;
    steeringPID.i = 0;
    steeringPID.d = 0;
    steeringPID.output = 0;
}

void steeringOn() {
    steeringPID.onoff = PID_ON;
    steeringPID.p = 0;
    steeringPID.i = 0;
    steeringPID.d = 0;
    steeringPID.output = 0;
}

