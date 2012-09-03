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
#include "ams-enc.h"
#include "motor_ctrl.h"
#include "gyro.h"
#include "dfilter_avg.h"
#include "imu.h"
#include <stdlib.h> // for malloc

#define ABS(my_val) ((my_val) < 0) ? -(my_val) : (my_val)
//#define PIDOUT2HBRIDGETORQUE 0.00305185095 //convert signed 16 bit integer to -100 to 100
#define PIDOUT2HBRIDGETORQUE 0.1

#define POS 1
#define NEG -1
#define ZERO 0

#define MAXTAILPOSITION 130.0
#define MINTAILPOSITION -115.0

//#define MAXTAILPOSITION 120.0
//#define MINTAILPOSITION -100.0


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
float lastTailPos;
float tailTorque = 0.0;

float bodyPosDeadband = 5.0;
float bodyPosition = 0.0;
//float dt = 0.001;
// how do I get this from python
float refBodyPosition = 0.0;
float initialBodyPosition = 0.0;
int gyroCtrlTorque;
int tailCtrlFlag = 0;

 

//Function to be installed into T1, and setup function
static void SetupTimer1(void);
static void tailCtrlServiceRoutine(void); //To be installed with sysService
//The following local functions are called by the service routine:
static void serviceTailQueue(void);
static void tailSynth();
static void serviceTailPID();

//volatile char tailInMotion;

/////////        Leg Control ISR       ////////
/////////  Installed to Timer1 @ 1Khz  ////////
//void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {




static void tailCtrlServiceRoutine(void) {
    serviceTailQueue(); //Update controllers
    tailSynth();
    serviceTailPID();
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
    

    //Tail queue
    tailq = tailqInit(16);
    idleTail = malloc(sizeof (tailCmdStruct));
    idleTail->angle = 0.0;
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
            TAIL_DEFAULT_KD, TAIL_DEFAULT_KAW, TAIL_DEFAULT_KFF);
    tailPID.satValPos = 32767;
    tailPID.satValNeg = -32768;
    tailPID.maxVal = 32767;
    tailPID.minVal = -32768;

    tailPID.onoff = PID_ON;

	int retval;
    retval = sysServiceInstallT1(tailCtrlServiceRoutine);
 
}


////// Tail functions below here

static void serviceTailQueue(void) {
    //Service Move Queue if not empty
    if (!tailqIsEmpty(tailq)) {
        //tailInMotion = 1;
        if ((currentTail == idleTail) || (getT1_ticks() >= tailExpire)) {
            currentTail = tailqPop(tailq); //grabs top command from tail queue
            initialBodyPosition = imuGetBodyZPositionDeg();
            tailExpire = getT1_ticks() + currentTail->duration;
            currentTailStart = getT1_ticks();

            //If we are no on an Idle move, turn on controllers
            if (currentTail->type != TAIL_SEG_IDLE) {
                //TODO: Turn on tail controller
				tailPID.onoff = PID_ON;
            }
        }
    }//Move Queue is empty
    else if ((getT1_ticks() >= tailExpire) && currentTail != idleTail) {
        //No more moves, go back to idle
        currentTail = idleTail;
        //TODO: Zero tail torque, turn off controller
		tailPID.onoff = PID_OFF;
		tailPID.output = 0;
        tailExpire = 0;
    }
}

static void tailSynth() {

    //int gyroAvg[3]; int gyroData[3]; int gyroOffsets[3];
    //Move segment synthesis
    long yS = (int) (10.0*currentTail->angle); //store in local variable to limit lookups
    int y = 0;
        if (currentTail->type == TAIL_SEG_IDLE) {
            y = 0;
			tailPID.onoff = PID_OFF;
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

		// New code July 31 2012

		//turn the tail until the body reaches a reference position
		if (currentTail->type == TAIL_GYRO_CONTROL) {
			//Set PID input to zero, turn PID off
			y = 0;
			tailPID.onoff = PID_OFF;
			
			//Throw a flag so you don't use the PID control
			tailCtrlFlag = 1;



			refBodyPosition = (float) currentTail->params[0];
                            // nothing
			bodyPosition = imuGetBodyZPositionDeg();// - initialBodyPosition;

			if (bodyPosition < (refBodyPosition - bodyPosDeadband)) {
				gyroCtrlTorque = POS; 
			}	
                        else if (bodyPosition > (refBodyPosition + bodyPosDeadband)) {
				gyroCtrlTorque = NEG; 
			}
                        else {
                            gyroCtrlTorque = ZERO;
                        }

			// if tail angle is at a max or min, apply no torque
                       /* if (lastTailPos < MINTAILPOSITION) {
                            gyroCtrlTorque = ZERO;
                        }

                         if (lastTailPos > MAXTAILPOSITION) {
                            gyroCtrlTorque = ZERO;
                        }
                        */
                        }
			
 
                        
        //TODO: Set tail input here
        tailPID.input = y;
    
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



static void serviceTailPID() {

	//update tail position
	lastTailPos = encGetAux1Pos();
	
	int encAngle = (int) (lastTailPos*10.0);
	//Update the setpoints
    //if((currentMove->inputL != 0) && (currentMove->inputR != 0)){
    if (currentTail != idleTail) {
        //Only update steering controller if we are in motion


#ifdef PID_SOFTWARE
        pidUpdate(&tailPID, encAngle);
#elif defined PID_HARDWARE
        int temp = 0;
        temp = tailPID.input; //Save unscaled input val
        tailPID.input *= TAIL_PID_SCALER; //Scale input
        pidUpdate(&tailPID,
                 TAIL_PID_SCALER * encAngle); //Update with scaled feedback, sets tailPID.output
       tailPID.input = temp;  //Reset unscaled input
		

#endif   //PID_SOFTWWARE vs PID_HARDWARE

	
	}
	else {
		tailPID.output = 0; //no output if idling
    }

	tailTorque = tailPID.output*PIDOUT2HBRIDGETORQUE;

	if(tailTorque > 100.0) {
		tailTorque = 100.0;
	}

	if(tailTorque < -100.0) {
		tailTorque = -100.0;
	}

	
	if (tailCtrlFlag == 1) {

        // HOW DO I DO THIS "AND" CORRECTLY IN C

        if (lastTailPos > MINTAILPOSITION && lastTailPos < MAXTAILPOSITION) {
            if (gyroCtrlTorque == POS) {
                mcSteer(-100.0);
            }
            if (gyroCtrlTorque == NEG) {
                mcSteer(100.0);
            }
            if (gyroCtrlTorque == ZERO) {
                mcSteer(0.0);
            }
        } else {
            mcSteer(0.0);
        }
    } else {

        mcSteer(tailTorque);
    } //if tailCtrlFlag == 1

}

