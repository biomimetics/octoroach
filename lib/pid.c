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
unsigned long lastMoveTime, moveExpire;
int seqIndex;
volatile int ADC_OffsetL, ADC_OffsetR;

unsigned long offsetAccumulatorL, offsetAccumulatorR;
unsigned int offsetAccumulatorCounter;
int blinkCtr;

#define PID_ZEROING_ENABLE 1

MoveQueue moveq;
moveCmdT currentMove, idleMove;

//
//unsigned int* readings_ptr;
int measurements[NUM_PIDS];
int measLast[NUM_PIDS];
int bemf[NUM_PIDS]; //used to store the true, unfiltered speed

static struct piddata {
    int output[NUM_PIDS];
    unsigned int measurements[NUM_PIDS];
	long p[2],i[2],d[2];
} PIDTelemData;


void pidSetup()
{
	int i;
	for(i = 0; i < NUM_PIDS; i++){
		initPIDObj( &(pidObjs[i]), DEFAULT_KP, DEFAULT_KI, DEFAULT_KD, DEFAULT_KAW, DEFAULT_FF); 
	}
	pidObjs[0].OUTPUT_CHANNEL = MC_CHANNEL_PWM1;
	pidObjs[1].OUTPUT_CHANNEL = MC_CHANNEL_PWM2;

	SetupTimer1();
	ADC_OffsetL = 1; //prevent divide by zero errors
	ADC_OffsetR = 1;

	moveq = mqInit(64);
	idleMove = malloc(sizeof(moveCmdStruct));
	idleMove->inputL = 0;
	idleMove->inputR = 0;
	idleMove->duration = 0;
	currentMove = idleMove;

	lastMoveTime = 0;
	moveExpire = 0;
	blinkCtr = 0;
	
	calibBatteryOffset(100);

	//Set which PWM output each PID Object will correspond to
	

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
	//Seed the IIR filter
	measLast[pid_num] =input_val;
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
	PIDTelemData.measurements[0] = measurements[0];
	PIDTelemData.measurements[1] = measurements[1];
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
	ConfigIntTimer1(T1_INT_PRIOR_4 & T1_INT_ON);
}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {

    //unsigned int* readings_ptr;
    //unsigned int measurements[NUM_PIDS];
    //int errors[NUM_PIDS];
    int command[NUM_PIDS];

    //LATB ^= (1<<4);
	
	//Blink red LED when executing move program
	if(currentMove != idleMove){
		if(blinkCtr == 0){
			blinkCtr = 100;
			LED_RED = ~LED_RED;
		}
		blinkCtr--;
	}

	//Service Move Queue
	if(!mqIsEmpty(moveq) )
	{
		if((currentMove == idleMove) || (t1_ticks >= moveExpire) )
		{
			currentMove = mqPop(moveq);
			moveExpire = t1_ticks + currentMove->duration;
			pidSetInput(0, currentMove->inputL, currentMove->duration);
			if(currentMove->inputL > 0){ pidObjs[0].onoff = 1; }
			pidSetInput(1, currentMove->inputR, currentMove->duration);
			if(currentMove->inputR > 0){ pidObjs[1].onoff = 1; }
		}
	}
	else if((t1_ticks >= moveExpire) && currentMove != idleMove){
		//No more moves, go back to idle
		currentMove = idleMove;
		pidSetInput(0, 0, 0);
		pidObjs[0].onoff = 0;
		pidSetInput(1, 0, 0);
		pidObjs[1].onoff = 0;
		moveExpire = 0;
		LED_RED = 1;
	} //else{
		//moveExpire = 0;
	//}
	
	
    //Get motor speed reading on every interrupt
    while(BusyADC1());
    //measurements[0] = ADC_Offset - ReadADC1(0);
	measurements[0] = pidObjs[0].inputOffset - ReadADC1(0);
    //measurements[0] = ADC_MAX - ReadADC1(0);
    offsetAccumulatorL += ReadADC1(0);  //runs constantly, but only used at startup time

    while(BusyADC2());
    //measurements[1] = ADC_Offset - ReadADC2(0);
	measurements[1] = pidObjs[1].inputOffset - ReadADC2(0);
    //measurements[1] = ADC_MAX - ReadADC2(0);
	offsetAccumulatorR += ReadADC2(0);   //runs constantly, but only used at startup time

	offsetAccumulatorCounter++;

    //Negative ADC measures mean nothing and screw up the math, maybe?
    if(measurements[0] < 0) { measurements[0] = 0;}
    if(measurements[1] < 0) { measurements[1] = 0;}

	// IIR filter: y[n] = 0.8 * y[n-1] + 0.2 * x[n]
	bemf[0] = measurements[0];
	bemf[1] = measurements[1];
	measurements[0] = (8 * (long)measLast[0] / 10) + 2 * (long)measurements[0] / 10;
	measurements[1] = (8 * (long)measLast[1] / 10) + 2 * (long)measurements[1] / 10;
	measLast[0] = measurements[0];
	measLast[1] = measurements[1];


    //if((measurements[0] > 0) || (measurements[1] > 0)) {
    if((measurements[0] > 0) || (measurements[1] > 0)) {
            LED_YELLOW = 1;}
    else{
            LED_YELLOW = 0;}
    int j;
    for(j=0; j < NUM_PIDS; j++){
        //pidobjs[0] : Left side
		//pidobjs[0] : Right side
        if(pidObjs[j].onoff){

            //Might want to change this in the future, if we want to track error
            //even when the motor is off.
            pidObjs[j].error = (long)pidObjs[j].input - (long)measurements[j];
            //Update values
            UpdatePID(&(pidObjs[j]) , measurements[j]);

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


	t1_ticks++;

    //Clear Timer1 interrupt flag
    _T1IF = 0;
}

void pidOn(int pid_num){
	pidObjs[pid_num].onoff = 1;
}

void calibBatteryOffset(int spindown_ms){
	int tempPDC1 = PDC1;
	int tempPDC2 = PDC2;
	unsigned long temp;
	PDC1 = 0; PDC2 = 0;
	short tempPidObjsOnOff[NUM_PIDS];
	tempPidObjsOnOff[0] = pidObjs[0].onoff;
	tempPidObjsOnOff[1] = pidObjs[1].onoff;
	pidObjs[0].onoff = 0; pidObjs[1].onoff = 0;
	pidObjs[0].onoff = 0;
	pidObjs[0].onoff = 0;
	delay_ms(spindown_ms); //motor spin-down
	LED_RED = 1;
	offsetAccumulatorL = 0;
	offsetAccumulatorR = 0; 
	offsetAccumulatorCounter = 0;
	while(offsetAccumulatorCounter < 100);
	//Left
	temp = offsetAccumulatorL;
	temp = temp/(unsigned long)offsetAccumulatorCounter;
	//ADC_OffsetL = temp-1;
	pidObjs[0].inputOffset = temp-1;
	//Right
	temp = offsetAccumulatorR;
	temp = temp/(unsigned long)offsetAccumulatorCounter;
	//ADC_OffsetR = temp-1;
	pidObjs[1].inputOffset = temp-1;

	LED_RED = 0;

	PDC1 = tempPDC1;
	PDC2 = tempPDC2;
	pidObjs[0].onoff = tempPidObjsOnOff[0];
	pidObjs[1].onoff = tempPidObjsOnOff[1];
}
