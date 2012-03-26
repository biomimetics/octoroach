/*
 * Name: UpdatePID.c
 * Desc: Control code to compute the new input to the plant
 * Date: 2009-04-03
 * Author: AMH
 */
#include "pid.h"
#include "motor_ctrl.h"
#include "timer.h"
#include "adc_pid.h"
#include "pwm.h"
#include "led.h"
#include "adc.h"
#include "move_queue.h"
#include "math.h"
#include "steering.h"

#include <stdlib.h> // for malloc

//#define HALFTHROT 10000
#define HALFTHROT 2000
#define FULLTHROT 2*HALFTHROT
//#define MAXTHROT 19878
#define MAXTHROT 3976
#define UINT_MAX 65535

#define ABS(my_val) ((my_val) < 0) ? -(my_val) : (my_val)


pidT pidObjs[NUM_PIDS];
volatile unsigned long t1_ticks;
unsigned long currentMoveStart, moveExpire;
int seqIndex;
volatile int ADC_OffsetL, ADC_OffsetR;

unsigned long offsetAccumulatorL, offsetAccumulatorR;
unsigned int offsetAccumulatorCounter;
int blinkCtr;

//This is an option to force the PID outputs back to zero when there is no input.
//This was an attempt to stop bugs w/ motor twitching, or controller wandering.
//It may not be needed anymore.
#define PID_ZEROING_ENABLE 1

MoveQueue moveq;
moveCmdT currentMove, idleMove;

int bemf[NUM_PIDS]; //used to store the true, unfiltered speed
int bemfLast[NUM_PIDS];


volatile char inMotion;

//Local scope functions
void serviceMoveQueue(void);
static void moveSynth();

static struct piddata {
    int output[NUM_PIDS];
    unsigned int measurements[NUM_PIDS];
	long p[2],i[2],d[2];
} PIDTelemData;


//////////////////////////


void pidSetup()
{
	int i;
	for(i = 0; i < NUM_PIDS; i++){
		initPIDObj( &(pidObjs[i]), DEFAULT_KP, DEFAULT_KI, DEFAULT_KD, DEFAULT_KAW, DEFAULT_FF); 
	}
	//Set which PWM output each PID Object will correspond to
	pidObjs[0].OUTPUT_CHANNEL = MC_CHANNEL_PWM1;
	pidObjs[1].OUTPUT_CHANNEL = MC_CHANNEL_PWM2;

	SetupTimer1();
	ADC_OffsetL = 1; //prevent divide by zero errors
	ADC_OffsetR = 1;

	moveq = mqInit(8);
	idleMove = malloc(sizeof(moveCmdStruct));
	idleMove->inputL = 0;
	idleMove->inputR = 0;
	idleMove->duration = 0;
	idleMove->type = MOVE_SEG_IDLE;
	idleMove->params[0]=0;idleMove->params[1]=0;idleMove->params[2]=0;
	currentMove = idleMove;

	currentMoveStart = 0;
	moveExpire = 0;
	blinkCtr = 0;
	inMotion = 0;
	
	//calibBatteryOffset(100);
	
	//Ensure controllers are reset to zero and turned off
	pidSetInput(0,0,0);
	pidSetInput(1,0,0);
	pidObjs[0].onoff = 0;
	pidObjs[1].onoff = 0;
}

void UpdatePID(pidT *pid, int y)
{
    pid->p = (long)pid->Kp * pid->error;
    pid->i = (long)pid->Ki * pid->iState;
    //Filtered derivative action applied directly to measurement
    pid->d = ((long)pid->Kd * (long)pid->d * (long)GAIN_SCALER) / ((long)pid->Kd + (long)pid->Kp * (long)pid->N) - 
        ((long)pid->Kd * (long)pid->Kp * (long)pid->N * ((long)y - (long)pid->y_old)) /
        ((long)pid->Kd + (long)pid->Kp * (long)pid->N);

    pid->preSat = pid->feedforward + ((pid->p + pid->i + pid->d) * (long)FULLTHROT) / ((long)GAIN_SCALER * (long)(pid->inputOffset));
	//pid->preSat = ((pid->p + pid->i + pid->d) * (long)FULLTHROT) / ((long)GAIN_SCALER * (long)ADC_Offset);
	//pid->preSat = (pid->p * (long)FULLTHROT) / ((long)GAIN_SCALER * (long)ADC_MAX);
    
	if (pid->preSat > MAXTHROT)
    {   
        pid->output = MAXTHROT;
    }else
    {
        pid->output = pid->preSat;
    }

    //pid->iState += (long)(pid->error) + (long)(pid->Kaw) * ((long)(pid->output) - (long)(pid->preSat));
	pid->iState += (long)(pid->error) + (long)(pid->Kaw) * ((long)(pid->output) - (long)(pid->preSat)) / ((long)GAIN_SCALER);
    pid->y_old = y;  
}

void initPIDObj(pidT *pid, int Kp, int Ki, int Kd, int Kaw, int ff)
{
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
    pid->Ki= Ki;
    pid->Kd = Kd;
    pid->Kaw = Kaw; 
	pid->feedforward = 0;
    pid->onoff = 0;
    pid->error = 0;
}

void pidSetInput(int pid_num, int input_val, unsigned int run_time){
	pidObjs[pid_num].input = input_val;
    pidObjs[pid_num].run_time = run_time;
    pidObjs[pid_num].start_time = t1_ticks;
    //zero out running PID values
    pidObjs[pid_num].iState = 0;
    pidObjs[pid_num].dState = 0;
    pidObjs[pid_num].p = 0;
    pidObjs[pid_num].i = 0;
    pidObjs[pid_num].d = 0;
	//Seed the IIR filter; (TODO)
	//measLast[pid_num] = input_val;
	bemfLast[pid_num] = input_val;
}

void pidSetInputSameRuntime(int pid_num, int input_val){
    pidObjs[pid_num].input = input_val;
    //zero out running PID values
    pidObjs[pid_num].iState = 0;
    pidObjs[pid_num].dState = 0;
    pidObjs[pid_num].p = 0;
    pidObjs[pid_num].i = 0;
    pidObjs[pid_num].d = 0;
}

void pidSetGains(int pid_num, int Kp, int Ki, int Kd, int Kaw, int ff){
    pidObjs[pid_num].Kp  = Kp;
    pidObjs[pid_num].Ki  = Ki;
    pidObjs[pid_num].Kd  = Kd;
    pidObjs[pid_num].Kaw = Kaw;
	pidObjs[pid_num].feedforward = ff;
}

unsigned char* pidGetTelemetry(void){
	PIDTelemData.output[0] = pidObjs[0].output;
	PIDTelemData.output[1] = pidObjs[1].output;
	PIDTelemData.measurements[0] = bemf[0];
	PIDTelemData.measurements[1] = bemf[1];
	PIDTelemData.p[0] = pidObjs[0].p;
	PIDTelemData.p[1] = pidObjs[1].p;
	PIDTelemData.i[0] = pidObjs[0].i;
	PIDTelemData.i[1] = pidObjs[1].i;
	PIDTelemData.d[0] = pidObjs[0].d;
	PIDTelemData.d[1] = pidObjs[1].d;
	return (unsigned char*)&PIDTelemData;
}


void SetupTimer1(void)
{
    unsigned int T1CON1value, T1PERvalue;
    T1CON1value = T1_ON & T1_SOURCE_INT & T1_PS_1_1 & T1_GATE_OFF &
                  T1_SYNC_EXT_OFF & T1_IDLE_CON;
                
    T1PERvalue = 0x9C40; //clock period = 0.001s = (T1PERvalue/FCY) (1KHz)
	//T1PERvalue = 0x9C40/2;
	t1_ticks = 0;
    OpenTimer1(T1CON1value, T1PERvalue);
	ConfigIntTimer1(T1_INT_PRIOR_6 & T1_INT_ON);
}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {

    int command[NUM_PIDS];

	serviceMoveQueue();
	moveSynth();

	//applySteeringCorrection();
	int presteer[2] = { pidObjs[0].input, pidObjs[1].input};
	int poststeer[2] = {0,0};
	steeringApplyCorrection(presteer, poststeer);
	pidObjs[0].input = poststeer[0];
	pidObjs[1].input = poststeer[1];
	if((pidObjs[0].input < 0) || (pidObjs[1].input < 0)){
		Nop();
		Nop();
	}

	//Back EMF measurements are made automatically by coordination of the ADC, PWM, and DMA.
	//Copy to local variables. Not strictly neccesary, just for clarity.
	//This **REQUIRES** that the divider on the battery & BEMF circuits have the same ratio.
	bemf[0] = adcGetVBatt() - adcGetBEMFL();
	bemf[1] = adcGetVBatt() - adcGetBEMFR();

    //Negative ADC measures mean nothing and screw up the math, maybe?
    if(bemf[0] < 0) { bemf[0] = 0;}
    if(bemf[1] < 0) { bemf[1] = 0;}

	// IIR filter: y[n] = 0.8 * y[n-1] + 0.2 * x[n]
	bemf[0] = (8 * (long)bemfLast[0] / 10) + 2 * (long)bemf[0] / 10;
	bemf[1] = (8 * (long)bemfLast[1] / 10) + 2 * (long)bemf[1] / 10;
	bemfLast[0] = bemf[0];
	bemfLast[1] = bemf[1];

	//Simple indicator if a leg is "in motion", via the yellow LED.
	//Not functionally necceasry; can be elimited to use the LED for something else.
    if((bemf[0] > 0) || (bemf[1] > 0)) {
            LED_YELLOW = 1;}
    else{
            LED_YELLOW = 0;}
    
	int j;
    for(j=0; j < NUM_PIDS; j++){

		//We are now measuring battery voltage directly via AN0,
		// so the input offset to each PID loop can actually be tracked, and needs
		// to be updated. This should compensate for battery voltage drooping over time.
		pidObjs[j].inputOffset = adcGetVBatt();

        //pidobjs[0] : Left side
		//pidobjs[0] : Right side
        if(pidObjs[j].onoff){

            //Might want to change this in the future, if we want to track error
            //even when the motor is off.
            pidObjs[j].error = (long)pidObjs[j].input - (long)bemf[j];
            //Update values
            UpdatePID(&(pidObjs[j]) , bemf[j]);

            //Clamp output to be great than 0
            if (pidObjs[j].output < 0){
                    command[j] = 0;
            }
            else {
                    command[j] = pidObjs[j].output;
            }

            //Set PWM duty cycle
			SetDCMCPWM(pidObjs[j].OUTPUT_CHANNEL, command[j], 0);

        } //end of if (on / off)
        else if(PID_ZEROING_ENABLE){ //if PID loop is off
			SetDCMCPWM(pidObjs[j].OUTPUT_CHANNEL, 0, 0);
        }

    } // end of for(j)

	//Timer1 runs at 1kHz; t1_ticks is a ms counter
	t1_ticks++;

    //Clear Timer1 interrupt flag
    _T1IF = 0;
}

void pidOn(int pid_num){
	pidObjs[pid_num].onoff = 1;
}


void serviceMoveQueue(void){

	//Blink red LED when executing move program
	if(currentMove != idleMove){
		if(blinkCtr == 0){
			blinkCtr = 100;
			LED_RED = ~LED_RED;
		}
		blinkCtr--;
	}

	//Service Move Queue if not empty
	if(!mqIsEmpty(moveq) )
	{
		inMotion = 1;
		if((currentMove == idleMove) || (t1_ticks >= moveExpire) )
		{
			currentMove = mqPop(moveq);
			moveExpire = t1_ticks + currentMove->duration;
			currentMoveStart = t1_ticks;
			steeringOn();
			//pidSetInput(0, currentMove->inputL, currentMove->duration);
			//if(currentMove->inputL > 0 || currentMove->type == MOVE_SEG_RAMP){
			if(currentMove->type != MOVE_SEG_IDLE){
				pidObjs[0].onoff = 1;
				pidObjs[1].onoff = 1; 
			}
			//pidSetInput(1, currentMove->inputR, currentMove->duration);
			//if(currentMove->inputR > 0 || currentMove->type == MOVE_SEG_RAMP){
			//	pidObjs[1].onoff = 1;
			//}
		}
	}
	//Move Queue is empty
	else if((t1_ticks >= moveExpire) && currentMove != idleMove){ 
		//No more moves, go back to idle
		currentMove = idleMove;
		pidSetInput(0, 0, 0);
		pidObjs[0].onoff = 0;
		pidSetInput(1, 0, 0);
		pidObjs[1].onoff = 0;
		moveExpire = 0;
		inMotion = 0; //for sleep, synthesis
		steeringOff();
	} 
}

static void moveSynth(){
	//Move segment synthesis
	long ySL = currentMove->inputL; //store in local variable to limit lookups
	long ySR = currentMove->inputR; // "
	int yL = 0;
	int yR = 0;
	if(inMotion){
		if(currentMove->type == MOVE_SEG_IDLE){
			yL = 0; yR = 0;
		}
		if(currentMove->type == MOVE_SEG_CONSTANT){
			yL = ySL;
			yR = ySR;
		}
		if(currentMove->type == MOVE_SEG_RAMP){
			long rateL = (long)currentMove->params[0];
			long rateR = (long)currentMove->params[1];
			//Do division last to prevent integer math underflow
			yL = rateL*((long)t1_ticks - (long)currentMoveStart)/1000 + ySL;
			yR = rateR*((long)t1_ticks - (long)currentMoveStart)/1000 + ySR;
		}
		if(currentMove->type == MOVE_SEG_SIN){
			float amp = (float)currentMove->params[0];
			float F = (float)currentMove->params[1] / 1000;
			#define BAMS16_TO_FLOAT 1/10430.367658761737
			float phase = BAMS16_TO_FLOAT*(float)currentMove->params[2]; //binary angle
			//Must be very careful about underflow & overflow here!
			//long arg = 2*BAMS16_PI*mF/100;
			//arg = arg*(t1_ticks - currentMoveStart)/10 - phase;
			float fyL = amp*sin(2*3.1415*F*(float)(t1_ticks - currentMoveStart)/1000  - phase) + ySL;
			float fyR = amp*sin(2*3.1415*F*(float)(t1_ticks - currentMoveStart)/1000  - phase) + ySR;

			int temp = (int)fyL;
			if(temp < 0){temp = 0;}
			yL = (unsigned int)temp;
			temp = (int)fyR;
			if(temp < 0){temp = 0;}
			yR = (unsigned int)temp;
			//unsigned int yL = amp*sin(arg) + ySL;
		}
		pidObjs[0].input = yL;
		pidObjs[1].input = yR;
		if((pidObjs[0].input < 0) || (pidObjs[1].input < 0)){
			Nop();
			Nop();
		}
	}
	//Note hhere that pidObjs[n].input is not set if !inMotion, in case another behavior wants to
	// set it.
}
