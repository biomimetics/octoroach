#include "dsp.h"

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

#define HALFTHROT 10000
#define FULLTHROT 2*HALFTHROT
#define MAXTHROT 19878
#define UINT_MAX 65535

#define ABS(my_val) ((my_val) < 0) ? -(my_val) : (my_val)


pidT pidObjs[NUM_PIDS];
volatile unsigned long t1_ticks;
unsigned int seqStartTime;
int seqIndex;
volatile int ADC_Offset;

unsigned long offsetAccumulator;
unsigned int offsetAccumulatorCounter;

tPID pidDSPobjs[NUM_PIDS];

#define PID_ZEROING_ENABLE 1

moveCmdT moveSeq[8];

//
//unsigned int* readings_ptr;
int measurements[NUM_PIDS];

static struct piddata {
    int output[NUM_PIDS];
    unsigned int measurements[NUM_PIDS];
	long p[2],i[2],d[2];
} PIDTelemData;


void pidSetup()
{
    PIDInit(&pidDSPobjs[0]);
    PIDInit(&pidDSPobjs[1]);
}

void UpdatePID(pidT *pid, int y)
{
	pid->p = (long)pid->Kp * pid->error;
    pid->i = (long)pid->Ki * pid->iState;
    //Filtered derivative action applied directly to measurement
    pid->d = ((long)pid->Kd * (long)pid->d * (long)GAIN_SCALER) / ((long)pid->Kd + (long)pid->Kp * (long)pid->N) -
        ((long)pid->Kd * (long)pid->Kp * (long)pid->N * ((long)y - (long)pid->y_old)) /
        ((long)pid->Kd + (long)pid->Kp * (long)pid->N);

    pid->preSat = ((pid->p + pid->i + pid->d) * (long)FULLTHROT) / ((long)GAIN_SCALER * (long)ADC_Offset);
	//pid->preSat = (pid->p * (long)FULLTHROT) / ((long)GAIN_SCALER * (long)ADC_MAX);

	if (pid->preSat > MAXTHROT)
    {
        pid->output = MAXTHROT;
    }else
    {
        pid->output = pid->preSat;
    }

	pid->iState += (long)(pid->error) + (long)(pid->Kaw) * ((long)(pid->output) - (long)(pid->preSat));
}

void initPIDObj(pidT *pid, int Kp, int Ki, int Kd, int Kaw)
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
}

void pidSetGains(int pid_num, int Kp, int Ki, int Kd, int Kaw){
	pidObjs[pid_num].Kp  = Kp;
	pidObjs[pid_num].Ki  = Ki;
	pidObjs[pid_num].Kd  = Kd;
	pidObjs[pid_num].Kaw = Kaw;
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
                  T1_SYNC_EXT_OFF & T1_INT_PRIOR_7;

    T1PERvalue = 0x9C40; //clock period = 0.001s = (T1PERvalue/FCY) (1KHz)
	//T1PERvalue = 0x9C40/2;
	t1_ticks = 0;
    OpenTimer1(T1CON1value, T1PERvalue);
}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {

	//unsigned int* readings_ptr;
	//unsigned int measurements[NUM_PIDS];
	//int errors[NUM_PIDS];
	int command[NUM_PIDS];

	//LATB ^= (1<<4);

	if (t1_ticks == UINT_MAX) t1_ticks = 0;
    t1_ticks++;

	/*if( t1_ticks >= seqStartTime + moveSeq[seqIndex].duration) {
		seqIndex = (seqIndex + 1) % 8;
		pidSetInput(0, moveSeq[seqIndex].inputL, moveSeq[seqIndex].duration);
		pidSetInput(1, moveSeq[seqIndex].inputR, moveSeq[seqIndex].duration);
		(pidObjs[0].input > 0) ? (pidObjs[0].onoff = 1) : (pidObjs[0].onoff = 0);
		(pidObjs[1].input > 0) ? (pidObjs[1].onoff = 1) : (pidObjs[1].onoff = 0);
		seqStartTime = t1_ticks;
	}*/

	//Get motor speed reading on every interrupt
	while(BusyADC1());
	measurements[0] = ADC_Offset - ReadADC1(0);
	//measurements[0] = ADC_MAX - ReadADC1(0);
	offsetAccumulator += ReadADC1(0); offsetAccumulatorCounter++;
	while(BusyADC2());
	measurements[1] = ADC_Offset - ReadADC2(0);
	//measurements[1] = ADC_MAX - ReadADC2(0);

	//Negative ADC measures mean nothing and screw up the math, maybe?
	if(measurements[0] < 0) { measurements[0] = 0;}
	if(measurements[1] < 0) { measurements[1] = 0;}

	//if((measurements[0] > 0) || (measurements[1] > 0)) {
	if((measurements[0] > 0)) {
		LED_BLUE = 1;}
	else{
		LED_BLUE = 0;}

	//pidobjs[0] : Left side
	if(pidObjs[0].onoff){

		//Might want to change this in the future, if we want to track error
		//even when the motor is off.
		pidObjs[0].error = (long)pidObjs[0].input - (long)measurements[0];
		//Update values
		UpdatePID(&(pidObjs[0]) , measurements[0]);
		pidObjs[0].y_old = measurements[0];

		//Clamp output above 0
		if (pidObjs[0].output < 0){
			command[0] = 0;
		}
		else {
			command[0] = pidObjs[0].output;
		}

		//Set PWM duty cycle
		SetDCMCPWM(MC_CHANNEL_PWM1, command[0], 0);

		//Should put this inside UpdatePID

        pidObjs[0].iState += (long)pidObjs[0].error +
							(long)pidObjs[0].Kaw * ((long)pidObjs[0].output - pidObjs[0].preSat);
		//For Derivative component
        pidObjs[0].y_old = measurements[0];

		//check time expiration
		//if(t1_ticks >= pidObjs[0].start_time + pidObjs[0].run_time) {
		//	pidObjs[0].input = 0;
		//	pidObjs[0].iState = 0;
		//	pidObjs[0].onoff = 0;
		//}
	}
	else if(PID_ZEROING_ENABLE){
		SetDCMCPWM(MC_CHANNEL_PWM1, 0, 0);
	} //if PID loop is off


	//pidobjs[1] : Right side
	if( pidObjs[1].onoff){

		//Might want to change this in the future, if we want to track error
		//even when the motor is off.
		pidObjs[1].error = (long)pidObjs[1].input - (long)measurements[1];
		//Update values
		UpdatePID(&(pidObjs[1]) , measurements[1]);
		pidObjs[1].y_old = measurements[1];

		//Clamp output above 0
		if (pidObjs[1].output < 0){
			command[1] = 0;
		}
		else {
			command[1] = pidObjs[1].output;
		}

		//Set PWM duty cycle
		SetDCMCPWM(MC_CHANNEL_PWM2, command[1], 0);

		//Should put this inside UpdatePID

        pidObjs[1].iState += (long)pidObjs[1].error +
							(long)pidObjs[1].Kaw * ((long)pidObjs[1].output - pidObjs[1].preSat);
		//For Derivative component
        pidObjs[1].y_old = measurements[1];

		//check time expiration
		//if(t1_ticks >= pidObjs[1].start_time + pidObjs[1].run_time) {
		//	pidObjs[1].input = 0;
		//	pidObjs[1].iState = 0;
		//	pidObjs[1].onoff = 0;
		//}
	}
	else if(PID_ZEROING_ENABLE){
		SetDCMCPWM(MC_CHANNEL_PWM2, 0, 0);
	} //if PID loop is off

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
	offsetAccumulator = 0;
	offsetAccumulatorCounter = 0;
	while(offsetAccumulatorCounter < 100);
	temp = offsetAccumulator;
	temp = temp/(unsigned long)offsetAccumulatorCounter;
	//ADC_Offset = temp;
	ADC_Offset = temp-1;
	LED_RED = 0;

	PDC1 = tempPDC1;
	PDC2 = tempPDC2;
	pidObjs[0].onoff = tempPidObjsOnOff[0];
	pidObjs[1].onoff = tempPidObjsOnOff[1];
}
