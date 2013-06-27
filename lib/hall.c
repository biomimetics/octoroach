// Modifications and additions to this file made by Andrew Pullin are copyright, 2013
// Copyrights are acknowledged for portions of this code extant before modifications by Andrew Pullin 
// Any application of BSD or other license to copyright content without the authors express approval
// is invalid and void.

// July 14, 2011
// authors: rfearing, apullin
// code for position feedback using hall sensors on left and right motor

#include "p33Fxxxx.h"
#include "led.h"
#include "pid.h"
#include "hall.h"
#include "adc_pid.h"
#include "gyro.h"
//#include "steering.h"
#include "motor_ctrl.h"
#include "timer.h"
#include "adc_pid.h"
#include "pwm.h"
#include "move_queue.h"
#include "hall.h"
#include "p33Fxxxx.h"
#include "incap.h" // input capture
#include "sys_service.h"
#include <stdlib.h> // for malloc

//Private Functions
static void SetupTimer1(void);
static void SetupTimer2(void);
static void SetupInputCapture(void);
static void hallUpdateBEMF(void);
static void hallUpdatePID(pidPos *pid);
int medianFilter3(int*);

//Function to be installed into T1, and setup function
static void hallServiceRoutine(void);

///////////////////////////////////
/////// Local variables ///////////
//////////////////////////////////
long old_right_time, right_time, right_delta; // time of last event
// unsigned long tic, toc;
long old_left_time, left_time, left_delta;
long motor_count[2]; // 0 = left 1 = right counts on sensor

MoveQueue hallMoveq;
moveCmdT hallCurrentMove, hallIdleMove, hallManualMove;

int hallbemf[NUM_HALL_PIDS]; //used to store the true, unfiltered speed
int hallbemfLast[NUM_HALL_PIDS]; // Last post-median-filter value
int hallbemfHist[NUM_HALL_PIDS][3]; //This is ONLY for applying the median filter to

//This is an array to map legCtrl controller to PWM output channels
int hallOutputChannels[NUM_HALL_PIDS];

// PID control structure
pidPos hallPIDObjs[NUM_HALL_PIDS];
//pidObj hallPIDObjs[NUM_HALL_PIDS];

// structure for reference velocity for leg
hallVelLUT hallPIDVel[NUM_HALL_PIDS];

// may be glitch in longer missions at rollover
unsigned long lastMoveTime;
int seqIndex;

static void hallGetSetpoint();
static void hallSetControl();

///////////////////////////////////
/////// Private Functions /////////
///////////////////////////////////

//Hall effect sensor has ~ 3 kHz rate max, so choose clock high enough
// choose clock period = 6.4 us, divide FCY by 256

// highest interrupt priority. runs at 1 kHZ
static void SetupTimer1(void)
{
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
 
//Timer 2 counts
// Set up just to provide a tick counter at 400 Hz
static void SetupTimer2(void) {
    unsigned int T2CON1value, T2PERvalue;
    T2CON1value = T2_ON & T2_SOURCE_INT & T2_PS_1_256 & T2_GATE_OFF;
    T2PERvalue = 0xffff; // max period 
    int retval;
    retval = sysServiceConfigT2(T2CON1value, T2PERvalue, T2_INT_PRIOR_5 & T2_INT_ON);
    //TODO: Put a soft trap here, conditional on retval
}


static void SetupInputCapture() {
    // RB4 and RB5 will be used for inputs
    _TRISB4 = 1; // set for input
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

//unsigned long toc;


/////////////////////////
///////   ISR's  /////////
/////////////////////////

// Input Capture 7: left legs
// Input Capture 8: right legs
// handler for right leg
void __attribute__((__interrupt__, no_auto_psv)) _IC8Interrupt(void) {
    // Insert ISR code here
    motor_count[0]++; // increment count for right side - neglect overflow/wrap around
    //right_time = (long) IC8BUF + ((long) (t2_ticks) << 16);
    right_time = (long) IC8BUF + (getT2_ticks() << 16);
    right_delta = right_time - old_right_time;
    old_right_time = right_time;

    LED_RED = ~LED_RED;
    IFS1bits.IC8IF = 0; // Clear CN interrupt
}

//handler for left leg
void __attribute__((__interrupt__, no_auto_psv)) _IC7Interrupt(void) {
    // Insert ISR code here
    motor_count[1]++; // increment count for right side - neglect overflow/wrap around

    //left_time = (long) IC7BUF + ((long) (t2_ticks) << 16);
    left_time = (long) IC7BUF + (getT2_ticks() << 16);
    left_delta = left_time - old_left_time;
    old_left_time = left_time;

    LED_GREEN = ~LED_GREEN;

    IFS1bits.IC7IF = 0; // Clear CN interrupt
}

/// Replaced by sys_service module
//void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void) {
//
//    t2_ticks++; // updates about every 400 ms

    //Clear Timer2 interrupt flag
//    _T2IF = 0;
//}




//////////////////////////////////
/////// Public Functions /////////
//////////////////////////////////


//Main hall effect sensor setup, called from main()
void hallSetup() {
    //Init of PID controller objects
    int i;
    for (i = 0; i < NUM_HALL_PIDS; i++) {
        hallInitPIDObjPos(&(hallPIDObjs[i]), DEFAULT_HALL_KP, DEFAULT_HALL_KI,
                DEFAULT_HALL_KD, DEFAULT_HALL_KAW, DEFAULT_HALL_FF);
        hallPIDObjs[i].minVal = 0;
        hallPIDObjs[i].satValNeg = 0;
        hallPIDObjs[i].maxVal = FULLTHROT;
        hallPIDObjs[i].satValPos = SATTHROT;
    }

    // Controller to PWM channel correspondance
    hallOutputChannels[0] = MC_CHANNEL_PWM1;
    hallOutputChannels[1] = MC_CHANNEL_PWM2;

    //Init for velocity profile objects
    hallInitPIDVelProfile();

    //System setup
    SetupTimer1();
    SetupTimer2(); // used for leg hall effect sensors
    SetupInputCapture(); // setup input capture for hall effect sensors
    int retval;
    retval = sysServiceInstallT1(hallServiceRoutine);

    // returns pointer to queue with 8 move entries
    hallMoveq = mqInit(8);
    hallIdleMove = malloc(sizeof (moveCmdStruct));
    hallIdleMove->inputL = 0;
    hallIdleMove->inputR = 0;
    hallIdleMove->duration = 0;
    hallCurrentMove = hallIdleMove;

    hallManualMove = malloc(sizeof (moveCmdStruct));
    hallManualMove->inputL = 0;
    hallManualMove->inputR = 0;
    hallManualMove->duration = 0;

    lastMoveTime = 0;
    //  initialize PID structures before starting Timer1
    hallPIDSetInput(0, 0, 0);
    hallPIDSetInput(1, 0, 0);

    for (i = 0; i < NUM_HALL_PIDS; i++) {
        hallbemfLast[i] = 0;
        hallbemfHist[i][0] = 0;
        hallbemfHist[i][1] = 0;
        hallbemfHist[i][2] = 0;
    }
}

// ----------   all the initializations  -------------------------
// set expire time for first segment in pidSetInput - use start time from MoveClosedLoop
// set points and velocities for one revolution of leg
// called from pidSetup()

void hallInitPIDVelProfile() {
    int i, j;
    for (j = 0; j < NUM_PIDS; j++) {
        hallPIDVel[j].index = 0; // point to first velocity
        hallPIDVel[j].interpolate = 0;
        hallPIDVel[j].leg_stride = 0; // set initial leg count
        // set control intervals during stride - try to get close to 21.3 ratio (use 42 counts)
        hallPIDVel[j].interval[0] = (4 * STRIDE_TICKS / NUM_VELS / 3);
        hallPIDVel[j].delta[0] = 11;
        hallPIDVel[j].interval[1] = (2 * STRIDE_TICKS / NUM_VELS / 3);
        hallPIDVel[j].delta[1] = 10;
        hallPIDVel[j].interval[2] = (4 * STRIDE_TICKS / NUM_VELS / 3);
        hallPIDVel[j].delta[2] = 11;
        hallPIDVel[j].interval[3] = (2 * STRIDE_TICKS / NUM_VELS / 3);
        hallPIDVel[j].delta[3] = 10;
        for (i = 0; i < NUM_VELS; i++) { // interpolate values between setpoints, <<4 for resolution
            hallPIDVel[j].vel[i] = (hallPIDVel[j].delta[i] << 8) / hallPIDVel[j].interval[i];
        }
        hallPIDObjs[j].p_input = 0; // initialize first set point
    }
}

// set values from packet - leave previous motor_count, p_input, etc.
// called from cmd.c

void hallSetVelProfile(int pid_num, int *interval, int *delta, int *vel) {
    int i;
    for (i = 0; i < NUM_VELS; i++) {
        hallPIDVel[pid_num].interval[i] = interval[i];
        hallPIDVel[pid_num].delta[i] = delta[i];
        hallPIDVel[pid_num].vel[i] = vel[i];
    }
}


// called from set thrust closed loop, etc. Thrust

void hallPIDSetInput(int pid_num, int input_val, unsigned int run_time) {
    unsigned long temp;
    hallPIDObjs[pid_num].v_input = input_val;
    hallPIDObjs[pid_num].run_time = run_time;
    hallPIDObjs[pid_num].start_time = getT1_ticks();
    //zero out running PID values
    hallPIDObjs[pid_num].i_error = 0;
    hallPIDObjs[pid_num].p = 0;
    hallPIDObjs[pid_num].i = 0;
    hallPIDObjs[pid_num].d = 0;
    //Seed the median filter

    temp = getT1_ticks(); // need atomic read due to interrupt
    lastMoveTime = temp + (unsigned long) run_time; // only one run time for both sides
    // set initial time for next move set point

    /*   need to set index =0 initial values */
    /* position setpoints start at 0 (index=0), then interpolate until setpoint 1 (index =1), etc */
    hallPIDVel[pid_num].expire = temp + (long) hallPIDVel[pid_num].interval[0]; // end of first interval
    hallPIDVel[pid_num].interpolate = 0;
    /*	pidObjs[pid_num].p_input += pidVel[pid_num].delta[0];	//update to first set point
     ***  this should be set only after first .expire time to avoid initial transients */
    hallPIDVel[pid_num].index = 0; // reset setpoint index
    // set first move at t = 0
    //	pidVel[0].expire = temp;   // right side
    //	pidVel[1].expire = temp;   // left side

}


void hallSetInputSameRuntime(int pid_num, int input_val) {
    hallPIDObjs[pid_num].v_input = input_val;
    //zero out running PID values
    hallPIDObjs[pid_num].i_error = 0;
    hallPIDObjs[pid_num].p = 0;
    hallPIDObjs[pid_num].i = 0;
    hallPIDObjs[pid_num].d = 0;
}


void hallSetGains(int pid_num, int Kp, int Ki, int Kd, int Kaw, int Kff) {
    hallPIDObjs[pid_num].Kp = Kp;
    hallPIDObjs[pid_num].Ki = Ki;
    hallPIDObjs[pid_num].Kd = Kd;
    hallPIDObjs[pid_num].Kaw = Kaw;
    hallPIDObjs[pid_num].Kff = Kff;
}

void hallPIDOn(int pid_num) {
    hallPIDObjs[pid_num].onoff = 1;
}

// zero position setpoint for both motors (avoids big offset errors)

void hallZeroPos(int pid_num) {
    // disable interrupts to reset both motor_counts
    DisableIntIC7;
    DisableIntIC8;
    motor_count[0] = 0;
    motor_count[1] = 0;
    EnableIntIC7;
    EnableIntIC8;
    // reset position setpoint as well
    hallPIDObjs[pid_num].p_input = 0;
    hallPIDVel[pid_num].leg_stride = 0; // strides also reset
}


/*********************** Motor Control Interrupt *********************************************/
/*****************************************************************************************/

/*****************************************************************************************/


/////////        Hall Control ISR       ////////
/////////  Installed to Timer1 @ 1Khz  ////////
//void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
static void hallServiceRoutine(void)
{
    // debug Hall effect sensors //
    LED_GREEN = _RB4;
    LED_RED = _RB5;

    if (getT1_ticks() > lastMoveTime) // turn off if done running
    { //	hallPIDSetInput(0, 0, 0);    don't reset state when done run, keep for recording telemetry
        hallPIDObjs[0].onoff = 0;
        //	hallPIDSetInput(1, 0, 0);
        hallPIDObjs[1].onoff = 0;
    } else // update velocity setpoints if needed - only when running
    {
        hallGetSetpoint();
    }

    hallUpdateBEMF();
    hallSetControl();
}

static void hallGetSetpoint() {
    int j, index;

    for (j = 0; j < NUM_HALL_PIDS; j++) {
        index = hallPIDVel[j].index;
        // update desired position between setpoints, scaled by 256
        hallPIDVel[j].interpolate += hallPIDVel[j].vel[index];

        if (getT1_ticks() >= hallPIDVel[j].expire) // time to reach previous setpoint has passed
        {
            hallPIDVel[j].interpolate = 0;
            hallPIDObjs[j].p_input += hallPIDVel[j].delta[index]; //update to next set point
            hallPIDVel[j].expire += hallPIDVel[j].interval[(index + 1) % NUM_VELS]; // expire time for next interval
            // got to next index point
            hallPIDVel[j].index++;

            if (hallPIDVel[j].index >= NUM_VELS) {
                hallPIDVel[j].index = 0;
                hallPIDVel[j].leg_stride++; // one full leg revolution
                // need to correct for 426 counts per leg stride
                // 5 rev @ 42 counts/rev = 210, actual set point 5 rev @ 42.6 counts, so add 3 to p_input
                if ((hallPIDVel[j].leg_stride % 5) == 0) {
                    hallPIDObjs[j].p_input += 3;
                }
            } // loop on index
        }
    }
}

static void hallSetControl() {
    int j;
    // 0 = right side
    for (j = 0; j < NUM_HALL_PIDS; j++) { //pidobjs[0] : right side
        // p_input has scaled velocity interpolation to make smoother
        hallPIDObjs[j].p_error = hallPIDObjs[j].p_input + (hallPIDVel[j].interpolate >> 8) - motor_count[j];
        //hallPIDObjs[j].v_error = hallPIDObjs[j].v_input - measurements[j];
        hallPIDObjs[j].v_error = hallPIDObjs[j].v_input - hallbemf[j];
        //Update values
        hallUpdatePID(&(hallPIDObjs[j]));
        if (hallPIDObjs[j].onoff) {
            //Might want to change this in the future, if we want to track error
            //even when the motor is off.
            //Set PWM duty cycle
            if (j == 0) { // PWM1.L
                SetDCMCPWM(MC_CHANNEL_PWM1, hallPIDObjs[0].output, 0); //PWM1.L
            } else if (j == 1) { // PWM2.l
                SetDCMCPWM(MC_CHANNEL_PWM2, hallPIDObjs[1].output, 0); // PWM2.L
            }
        }//end of if (on / off)
        else { //if PID loop is off
            if (j == 0) {
                SetDCMCPWM(MC_CHANNEL_PWM1, 0, 0);
            } else if (j == 1) {
                SetDCMCPWM(MC_CHANNEL_PWM2, 0, 0);
            }
        }
    } // end of for(j)
}

static void hallUpdatePID(pidPos *pid) {
    pid->p = (long) pid->Kp * pid->p_error;
    pid->i = (long) pid->Ki * pid->i_error;
    pid->d = (long) pid->Kd * (long) pid->v_error;
    // better check scale factors
    /* just use simpled PID, offset is already subtracted in PID GetState */
    // scale so doesn't over flow
    pid->preSat = pid->Kff + pid->p +
            ((pid->i + pid->d) >> 4); // divide by 16
    pid->output = pid->preSat;
    //Clamp output above 0 since don't have H bridge
    if (pid->preSat < pid->minVal) {
        pid->output = pid->minVal;
    }

    pid-> i_error = (long) pid-> i_error + (long) pid->p_error; // integrate error
    // saturate output - assume only worry about >0 for now
    // apply anti-windup to integrator
    if (pid->preSat > pid->satValPos) {
        pid->output = pid->satValPos;
        pid->i_error = (long) pid->i_error + (long) (pid->Kaw) * ((long) (pid->satValPos) - (long) (pid->preSat)) / ((long) GAIN_SCALER);
    }

}

//This duplicates functionalist in leg_ctrl
static void hallUpdateBEMF() {
    //Back EMF measurements are made automatically by coordination of the ADC, PWM, and DMA.
    //Copy to local variables. Not strictly neccesary, just for clarity.
    //This **REQUIRES** that the divider on the battery & BEMF circuits have the same ratio.
    hallbemf[0] = adcGetVBatt() - adcGetBEMFL();
    hallbemf[1] = adcGetVBatt() - adcGetBEMFR();
    //NOTE: at this point, we should have a proper correspondance between
    //   the order of all the structured variable; bemf[i] associated with
    //   pidObjs[i], bemfLast[i], etc.
    //   Any "jumbling" of the inputs can be done in the above assignments.

    //Negative ADC measures mean nothing and should never happen anyway
    if (hallbemf[0] < 0) {
        hallbemf[0] = 0;
    }
    if (hallbemf[1] < 0) {
        hallbemf[1] = 0;
    }

    //Apply median filter
    int i;
    for (i = 0; i < NUM_HALL_PIDS; i++) {
        hallbemfHist[i][2] = hallbemfHist[i][1]; //rotate first
        hallbemfHist[i][1] = hallbemfHist[i][0];
        hallbemfHist[i][0] = hallbemf[i]; //include newest value
        hallbemf[i] = medianFilter3(hallbemfHist[i]); //Apply median filter
    }

    // IIR filter on BEMF: y[n] = 0.2 * y[n-1] + 0.8 * x[n]
    hallbemf[0] = (2 * (long) hallbemfLast[0] / 10) + 8 * (long) hallbemf[0] / 10;
    hallbemf[1] = (2 * (long) hallbemfLast[1] / 10) + 8 * (long) hallbemf[1] / 10;
    hallbemfLast[0] = hallbemf[0]; //bemfLast will not be used after here, OK to set
    hallbemfLast[1] = hallbemf[1];
}


////   Public functions
////////////////////////

void hallInitPIDObjPos(pidPos *pid, int Kp, int Ki, int Kd, int Kaw, int Kff) {
    pid->p_input = 0;
    pid->v_input = 0;
    pid->p = 0;
    pid->i = 0;
    pid->d = 0;
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->Kaw = Kaw;
    pid->Kff = Kff;
    pid->output = 0;
    pid->onoff = 0;
    pid->p_error = 0;
    pid->v_error = 0;
    pid->i_error = 0;
}

// called from steeringSetup()

void hallInitPIDObj(pidObj *pid, int Kp, int Ki, int Kd, int Kaw, int Kff) {
    pid->input = 0;
    pid->dState = 0;
    pid->iState = 0;
    pid->output = 0;
    pid->y_old = 0;
    pid->p = 0;
    pid->i = 0;
    pid->d = 0;
    //This is just a guess for the derivative filter time constant
    pid->N = 5;
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->Kaw = Kaw;
    pid->Kff = 0;
    pid->onoff = 0;
    pid->error = 0;
}

//todo: a getter should not return a pointer; this is unsafe behavior.
long* hallGetMotorCounts() {
    return motor_count;
}
