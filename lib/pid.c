/*
 * Name: UpdatePID.c
 * Desc: Control code to compute the new input to the plant
 * Date: 2009-04-03
 * Author: AMH
   modified to include hall effect sensor by RSF.
 * modified Dec. 2011 to include telemetry capture
 * modified Jan. 2012 to include median filter on back emf
 */
#include "pid.h"
#include "dfmem_extra.h"
#include "gyro.h"
#include "steering.h"
#include "motor_ctrl.h"
#include "timer.h"
#include "adc_pid.h"
#include "pwm.h"
#include "led.h"
#include "adc.h"
#include "move_queue.h"
#include "hall.h"
#include "p33Fxxxx.h"
#include "stopwatch.h"
#include "incap.h" // input capture
#include <stdlib.h> // for malloc

//#define HALFTHROT 10000
#define HALFTHROT 2000
#define FULLTHROT 2*HALFTHROT
//#define MAXTHROT 19878
// MAXTHROT has to allow enough time at end of PWM for back emf measurement
// was 3976
#define MAXTHROT 3800

#define ABS(my_val) ((my_val) < 0) ? -(my_val) : (my_val)

// PID control structure
pidPos pidObjs[NUM_PIDS];

// structure for reference velocity for leg
pidVelLUT  pidVel[NUM_PIDS];


#define T1_MAX 0xffffff  // max before rollover of 1 ms counter
// may be glitch in longer missions at rollover
volatile unsigned long t1_ticks;
unsigned long lastMoveTime;
int seqIndex;

//for battery voltage:
char calib_flag = 0;   // flag is set if doing calibration
unsigned long offsetAccumulatorL, offsetAccumulatorR;
unsigned int offsetAccumulatorCounter;

MoveQueue moveq;
moveCmdT currentMove, idleMove, manualMove;

//
//unsigned int* readings_ptr;
int measurements[NUM_PIDS];
// 2 last readings for median filter
int measLast1[NUM_PIDS];
int measLast2[NUM_PIDS];
int bemf[NUM_PIDS]; //used to store the true, unfiltered speed

static struct piddata {
    int output[NUM_PIDS];
    unsigned int measurements[NUM_PIDS];
	long p[2],i[2],d[2];
} PIDTelemData;

extern long motor_count[2]; // 1 = right, number of counts on hall sensor

// -------------------------------------------
// called from main()
void pidSetup()
{
	int i;
	for(i = 0; i < NUM_PIDS; i++){
		initPIDObjPos( &(pidObjs[i]), DEFAULT_KP, DEFAULT_KI, DEFAULT_KD, DEFAULT_KAW, DEFAULT_FF); 
	}
	initPIDVelProfile();
	SetupTimer1();  // used for leg motor PID
	SetupTimer2(); // used for leg hall effect sensors
	SetupInputCapture(); // setup input capture for hall effect sensors

// returns pointer to queue with 8 move entries
	moveq = mqInit(8); 
	idleMove = malloc(sizeof(moveCmdStruct));
	idleMove->inputL = 0;
	idleMove->inputR = 0;
	idleMove->duration = 0;
	currentMove = idleMove;
	
	manualMove = malloc(sizeof(moveCmdStruct));
	manualMove->inputL = 0;
	manualMove->inputR = 0;
	manualMove->duration = 0;

	lastMoveTime = 0;
//  initialize PID structures before starting Timer1
	pidSetInput(0,0,0);
	pidSetInput(1,0,0);

	EnableIntT1; // turn on pid interrupts
	
	calibBatteryOffset(100);
}



// ----------   all the initializations  -------------------------
// set expire time for first segment in pidSetInput - use start time from MoveClosedLoop
// set points and velocities for one revolution of leg
// called from pidSetup()
void initPIDVelProfile()
{ int i,j;
	for(j = 0; j < NUM_PIDS; j++)
	{    	pidVel[j].index = 0;  // point to first velocity
		pidVel[j].interpolate = 0; 
		pidVel[j].leg_stride = 0;  // set initial leg count
	// set control intervals during stride - try to get close to 21.3 ratio (use 42 counts)
  		pidVel[j].interval[0]= (4*STRIDE_TICKS/NUM_VELS/3); 
		pidVel[j].delta[0] =  11;
	     	pidVel[j].interval[1]= (2*STRIDE_TICKS/NUM_VELS/3); 
		pidVel[j].delta[1] =  10;
	     	pidVel[j].interval[2]= (4*STRIDE_TICKS/NUM_VELS/3); 
		pidVel[j].delta[2] =  11;
	  	pidVel[j].interval[3]= (2*STRIDE_TICKS/NUM_VELS/3); 
		pidVel[j].delta[3] =  10;
		for(i = 0; i < NUM_VELS; i++)
		{   	// interpolate values between setpoints, <<4 for resolution
			pidVel[j].vel[i] = (pidVel[j].delta[i] << 8) / pidVel[j].interval[i];
		 }
		pidObjs[j].p_input = 0; // initialize first set point 
//		pidObjs[j].v_input = 0; // initialize first velocity
		pidObjs[j].v_input = (int)( pidVel[j].vel[0] * K_EMF);	//initialize first velocity
	}
}

// set values from packet - leave previous motor_count, p_input, etc.
// called from cmd.c
void setPIDVelProfile(int pid_num, int *interval, int *delta, int *vel)
{ int i;
	for (i = 0; i < NUM_VELS; i++)
	{ 	pidVel[pid_num].interval[i]= interval[i];
		pidVel[pid_num].delta[i]= delta[i];
		pidVel[pid_num].vel[i]= vel[i];
	}
}

// called from pidSetup()
void initPIDObjPos(pidPos *pid, int Kp, int Ki, int Kd, int Kaw, int ff)
{
    pid->p_input = 0;
    pid->v_input = 0;
    pid->p = 0;
    pid->i = 0;
    pid->d = 0;
    pid->Kp = Kp;
    pid->Ki= Ki;
    pid->Kd = Kd;
    pid->Kaw = Kaw; 
	pid->feedforward = 0;
  pid->output = 0;
    pid->onoff = 0;
	pid->p_error = 0;
	pid->v_error = 0;
	pid->i_error = 0;
    pid->phase_offset = 0;
}

// called from steeringSetup()
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

// called from set thrust closed loop, etc. Thrust 
void pidSetInput(int pid_num, int input_val, unsigned int run_time){
unsigned long temp;	
/*      ******   use velocity setpoint + throttle for compatibility between Hall and Pullin code *****/
/* otherwise, miss first velocity set point */
    pidObjs[pid_num].v_input = input_val + (int)( pidVel[pid_num].vel[0] * K_EMF);	//initialize first velocity ;
    pidObjs[pid_num].run_time = run_time;
    pidObjs[pid_num].start_time = t1_ticks;
    //zero out running PID values
    pidObjs[pid_num].i_error = 0;
    pidObjs[pid_num].p = 0;
    pidObjs[pid_num].i = 0;
    pidObjs[pid_num].d = 0;
	//Seed the median filter
	measLast1[pid_num] =input_val;
	measLast2[pid_num] =input_val;
	temp = t1_ticks;  // need atomic read due to interrupt 
	lastMoveTime = temp + (unsigned long) run_time;  // only one run time for both sides
	// set initial time for next move set point
  
/*   need to set index =0 initial values */
/* position setpoints start at 0 (index=0), then interpolate until setpoint 1 (index =1), etc */
	pidVel[pid_num].expire = temp + (long) pidVel[pid_num].interval[0];   // end of first interval
	pidVel[pid_num].interpolate = 0;	
/*	pidObjs[pid_num].p_input += pidVel[pid_num].delta[0];	//update to first set point
***  this should be set only after first .expire time to avoid initial transients */
	pidVel[pid_num].index =0; // reset setpoint index
// set first move at t = 0
//	pidVel[0].expire = temp;   // right side
//	pidVel[1].expire = temp;   // left side

}

// from steering?
void pidSetInputSameRuntime(int pid_num, int input_val){
    pidObjs[pid_num].v_input = input_val;
    //zero out running PID values
    pidObjs[pid_num].i_error = 0;
    pidObjs[pid_num].p = 0;
    pidObjs[pid_num].i = 0;
    pidObjs[pid_num].d = 0;
}

// from cmd.c  PID set gains
void pidSetGains(int pid_num, int Kp, int Ki, int Kd, int Kaw, int ff){
    pidObjs[pid_num].Kp  = Kp;
    pidObjs[pid_num].Ki  = Ki;
    pidObjs[pid_num].Kd  = Kd;
    pidObjs[pid_num].Kaw = Kaw;
	pidObjs[pid_num].feedforward = ff;
}

void pidOn(int pid_num){
	pidObjs[pid_num].onoff = 1;
}

// zero position setpoint for both motors (avoids big offset errors)
void pidZeroPos(int pid_num){ 
// disable interrupts to reset both motor_counts 
	DisableIntIC7;   DisableIntIC8;
	motor_count[0] = 0; motor_count[1]=0;
	EnableIntIC7; EnableIntIC8;   	
// reset position setpoint as well
	pidObjs[pid_num].p_input = 0;
	pidObjs[pid_num].v_input = 0;
	pidVel[pid_num].leg_stride = 0; // strides also reset 
}

// from cmdGetPIDTelemetry
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


// calibrate A/D offset, using PWM synchronized A/D reads inside 
// timer 1 interrupt loop

void calibBatteryOffset(int spindown_ms){
	unsigned long temp;
// save current PWM config
	int tempPDC1 = PDC1;
	int tempPDC2 = PDC2;
	PDC1 = 0; PDC2 = 0;  /* SFR for PWM? */

// save current PID status, and turn off PID control
	short tempPidObjsOnOff[NUM_PIDS];
	tempPidObjsOnOff[0] = pidObjs[0].onoff;
	tempPidObjsOnOff[1] = pidObjs[1].onoff;
	pidObjs[0].onoff = 0; pidObjs[1].onoff = 0;

	delay_ms(spindown_ms); //motor spin-down
	LED_RED = 1;
	offsetAccumulatorL = 0;
	offsetAccumulatorR = 0; 
	offsetAccumulatorCounter = 0; // updated inside servo loop
	calib_flag = 1;  // enable calibration
	while(offsetAccumulatorCounter < 100); // wait for 100 samples
	calib_flag = 0;  // turn off calibration
	//Left
	temp = offsetAccumulatorL;
	temp = temp/(unsigned long)offsetAccumulatorCounter;
	pidObjs[0].inputOffset = temp;

	//Right
	temp = offsetAccumulatorR;
	temp = temp/(unsigned long)offsetAccumulatorCounter;
	pidObjs[1].inputOffset = temp;

	LED_RED = 0;
// restore PID values
	PDC1 = tempPDC1;
	PDC2 = tempPDC2;
	pidObjs[0].onoff = tempPidObjsOnOff[0];
	pidObjs[1].onoff = tempPidObjsOnOff[1];
}


/*****************************************************************************************/
/*****************************************************************************************/
/*********************** Stop Motor and Interrupts *********************************************/
/*****************************************************************************************/
/*****************************************************************************************/
void EmergencyStop(void)
{
	pidSetInput(0 ,0, 0);
	pidSetInput(1,0,0);
	DisableIntT1; // turn off pid interrupts
       SetDCMCPWM(MC_CHANNEL_PWM1, 0, 0);    // set PWM to zero
       SetDCMCPWM(MC_CHANNEL_PWM2, 0, 0); 
}
	

// -------------------------   control  loop section  -------------------------------

/* highest interrupt priority. runs at 1 kHZ */
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

/*********************** Motor Control Interrupt *********************************************/
/*****************************************************************************************/
/*****************************************************************************************/

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) 
{
//  unsigned long time_start, time_end; 
//	time_start =  swatchTic(); 

    if (t1_ticks == T1_MAX) t1_ticks = 0;
    t1_ticks++;
 // debug Hall effect sensors //
	LED_GREEN = _RB4;
	LED_RED =  _RB5;
	
	if(t1_ticks > lastMoveTime) // turn off if done running
	{ //	pidSetInput(0, 0, 0);    don't reset state when done run, keep for recording telemetry
		pidObjs[0].onoff = 0;
	//	pidSetInput(1, 0, 0);
		pidObjs[1].onoff = 0;
	}
	else 		// update velocity setpoints if needed - only when running
	{ pidGetSetpoint();}

	pidGetState(measurements);

       pidSetControl();
/* better read telemetry at slower tick rate - updated by steering servo */

//	time_end =  swatchToc();
    //Clear Timer1 interrupt flag
    _T1IF = 0;
}


void pidGetSetpoint()
{ int j, index;

	 for(j=0; j < NUM_PIDS; j++)
		{  	index = pidVel[j].index;		
			// update desired position between setpoints, scaled by 256
			pidVel[j].interpolate += pidVel[j].vel[index];

			if (t1_ticks >= pidVel[j].expire)  // time to reach previous setpoint has passed
			{ 	pidVel[j].interpolate = 0;	
				pidObjs[j].p_input += pidVel[j].delta[index];	//update to next set point
				pidVel[j].expire += pidVel[j].interval[(index+1) % NUM_VELS];  // expire time for next interval
			       pidObjs[j].v_input = (int)( pidVel[j].vel[(index+1) % NUM_VELS] * K_EMF);	  //update to next velocity 
				// got to next index point	
				pidVel[j].index++;
		
				if (pidVel[j].index >= NUM_VELS) 
				{     pidVel[j].index = 0;
					pidVel[j].leg_stride++;  // one full leg revolution
	// need to correct for 426 counts per leg stride
      // 5 rev @ 42 counts/rev = 210, actual set point 5 rev @ 42.6 counts, so add 3 to p_input 
			//		if ((pidVel[j].leg_stride % 5) == 0)
			//		{ pidObjs[j].p_input +=3; }					
				}  // loop on index

			}
		}
}


void pidSetControl()
{ int j;
int phase_error;
// 0 = right side

//KCP HACK
//Add a term to drive down the error between the sides based on hall effects
static int phase_error_sum = 0;

//if positive, motor 0 is ahead, if negative, motor 1 is ahead
phase_error = motor_count[0] - motor_count[1];
phase_error_sum += phase_error;

//Speed up and slow down motors accordingly
pidObjs[0].phase_offset = -phase_error*pidObjs[0].feedforward - phase_error_sum*20;
pidObjs[1].phase_offset = phase_error*pidObjs[1].feedforward + phase_error_sum*20;

    for(j=0; j < NUM_PIDS; j++)
   {  //pidobjs[0] : right side
	// p_input has scaled velocity interpolation to make smoother
        	pidObjs[j].p_error = pidObjs[j].p_input + (pidVel[j].interpolate >> 8)  - motor_count[j];
            pidObjs[j].v_error = pidObjs[j].v_input - measurements[j];  // v_input should be A/D volts per Hall count/ms
            //Update values
            UpdatePID(&(pidObjs[j]));
     } // end of for(j)

if(pidObjs[0].onoff)
{
    //Might want to change this in the future, if we want to track error
    //even when the motor is off.
    //Set PWM duty cycle

     SetDCMCPWM(MC_CHANNEL_PWM1, pidObjs[0].output, 0); //PWM1.L
     SetDCMCPWM(MC_CHANNEL_PWM2, pidObjs[1].output, 0); // PWM2.L
} //end of if (on / off)
 else
    { //if PID loop is off
        SetDCMCPWM(MC_CHANNEL_PWM1, 0, 0);
        SetDCMCPWM(MC_CHANNEL_PWM2, 0, 0);
        phase_error_sum = 0;
    }

}


void UpdatePID(pidPos *pid)
{
    pid->p = (long)pid->Kp * pid->p_error;  
    pid->i = (long)pid->Ki  * pid->i_error;
    pid->d=(long)pid->Kd *  (long) pid->v_error;
    // better check scale factors
/* just use simple PID, offset is already subtracted in PID GetState */
// scale so doesn't over flow
    pid->preSat = pid->p + pid->phase_offset + ((pid->i + pid->d) >> 4); // divide by 16
	pid->output = pid->preSat;
   //Clamp output above 0 since don't have H bridge
    if (pid->preSat < 0){pid->output=0;}
 
	pid-> i_error = (long)pid-> i_error + (long)pid->p_error; // integrate error
// saturate output - assume only worry about >0 for now
// apply anti-windup to integrator  
	if (pid->preSat > MAXTHROT) 
	{ 	      pid->output = MAXTHROT; 
			pid->i_error = (long) pid->i_error + (long)(pid->Kaw) * ((long)(MAXTHROT) - (long)(pid->preSat)) / ((long)GAIN_SCALER);		
	}                    
}

/* update state variables including motor position and velocity */
void pidGetState(int *measurements)
{	//Get motor speed reading on every interrupt - A/D conversion triggered by PWM timer to read Vm when transistor is off
// when motor is loaded, sometimes see motor short so that  bemf=offset voltage
// get zero sometimes - open circuit brush? Hence try median filter
	int i;
    while(BusyADC1());
 	measurements[0] = pidObjs[0].inputOffset - ReadADC1(0); // AN11
    while(BusyADC2());
  	measurements[1] = pidObjs[1].inputOffset - ReadADC2(0); // AN1
	
 // get battery offset with motor off at startup time
	if(calib_flag)
	{ 	offsetAccumulatorL += ReadADC1(0);  
		offsetAccumulatorR += ReadADC2(0);   
		offsetAccumulatorCounter++; 	}

    
   // if (inputOfset - ReadADC_ is less than zero, it means motor is turning backwards. This happens at low speed due to joint stiffness
/*****
   if(measurements[0] < 0) 
    {  measurements[0] = bemf[0];
     }
    if(measurements[1] < 0)
    { measurements[1] = bemf[1];}
 ****/
	// median filter
	for(i = 0; i<2; i++)
	{	if(measurements[i] > measLast1[i])	
		{	if(measLast1[i] > measLast2[i]) {bemf[i] = measLast1[i];}  // middle value is median
			else // middle is smallest
	     		{ if(measurements[i] > measLast2[i]) {bemf[i] = measLast2[i];} // 3rd value is median
	        	   else{ bemf[i] = measurements[i];}  // first value is median
            	}
      	}           
		else  // first is not biggest
		{	if(measLast1[i] < measLast2[i]) {bemf[i] = measLast1[i];}  // middle value is median
			else  // middle is biggest
	     		{    if(measurements[i] < measLast2[i]) {bemf[i] = measLast2[i];} // 3rd value is median
		     		else
				{ bemf[i] = measurements[i];  // first value is median			 
				}
			}
		}
	}

// store old values
	measLast2[0] = measLast1[0];  measLast1[0] = measurements[0];
	measLast2[1] = measLast1[1];  measLast1[1] = measurements[1];
      measurements[0] = bemf[0];  // use fiiltered value
     measurements[1] = bemf[1];  // use fiiltered value

    //if((measurements[0] > 0) || (measurements[1] > 0)) {
    if((measurements[0] > 0)) { LED_BLUE = 1;}
    else{ LED_BLUE = 0;}
}

