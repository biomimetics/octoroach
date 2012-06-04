#include "pid.h"
#include "timer.h"
#include "gyro.h"
#include "steering.h"
#include "stopwatch.h"
#include "dfmem_extra.h"
#include "move_queue.h"
#include "xl.h"
#include "utils.h" 
#define TIMER_FREQUENCY     200                 // 400 Hz
#define TIMER_PERIOD        1/TIMER_FREQUENCY
#define SKIP_COUNT          2

long gyro_accum;
int j;
int gdata[3];
//int acceldata[3];
// unsigned char* xl_data;
int xldata[3];  // accelerometer data 
pidT steeringPID;
int steeringIsOn;
int gyroAvg;
int telemSkip;

// structure to keep track of telemetry recording
TelemStruct TelemControl;
extern int samplesToSave;


int datactr;

int samplesToSave;

static int gyroWindow[GYRO_AVG_SAMPLES];

int windowIdx;

extern int offsx, offsy, offsz;

extern pidPos pidObjs[NUM_PIDS];
extern int bemf[NUM_PIDS];
extern long motor_count[2]; // 1 = right, number of counts on hall sensor

extern moveCmdT currentMove;

extern unsigned long t1_ticks; //needed to calculate new runtimes

void steeringSetup(void) {

    gyro_accum = 0;
    j = 0;
    setSampleSaveCount(0);   // turn off sampling by default unless enabled
    telemSkip = 1;  

//set up telemetry structure
	TelemControl.onoff = 0; 
	TelemControl.start = 0x0;
	TelemControl.count = 0x0;
	TelemControl.skip = 0x01;
    
    initPIDObj(&steeringPID, STEERING_KP, STEERING_KI, STEERING_KD, STEERING_KAW, 0);
    setSteeringAngRate(0);

    // period value = Fcy/(prescale*Ftimer)
    unsigned int con_reg, period;

    // prescale 1:64
    con_reg = T5_ON & T5_IDLE_STOP & T5_GATE_OFF & T5_PS_1_64 & T5_SOURCE_INT;
    // Period is set so that period = 5ms (200Hz), MIPS = 40
    //period = 3125; // 200Hz
	period = 2083; // ~300Hz

    //con_reg = T3_ON & T3_IDLE_STOP & T3_GATE_OFF & T3_PS_1_64 & T3_SOURCE_INT;
    // Period is set so that period = 1.25ms (800Hz), MIPS = 40
    //period = 6250; // 100Hz

    OpenTimer5(con_reg, period);

    // interrupt for reading gyro
    ConfigIntTimer5(T5_INT_PRIOR_3 & T5_INT_ON);

    //offs = (float*)(gyroGetCalibParam());
	steeringIsOn = 1;
	windowIdx = 0;
}

void __attribute__((interrupt, no_auto_psv)) _T5Interrupt(void) {
    
    //int gyroAvg;
	int left, right;
	int i;

    /*
	if(j < GYRO_AVG_SAMPLES){
        gyro_accum += (long)gdata[2] - (long)offsz;
        j++;
    }
    else{ //make steering decision
		  //this should happen at at freuqnecy of f_timer5 / GYRO_AVG_SAMPLES
        gyroAvg = (int)(gyro_accum / (long)GYRO_AVG_SAMPLES);
		gyro_accum = 0;
        j = 0;

		//apply calculated steering correction to drive
		if((pidObjs[0].input != 0) && (pidObjs[1].input != 0)){  
			//Only update steering controller if we are in motion
			steeringPID.error = steeringPID.input - gyroAvg;
        	UpdatePIDSteering(&steeringPID , gyroAvg);
			
			//left = pidObjs[0].input;    WRONG --> causes an implicit accumulation!!!
			//right = pidObjs[1].input; 
			left = currentMove->inputL;
			right = currentMove->inputR;
			
			// Depending on which way the bot is turning, choose which side to add correction to
			if( steeringPID.input <= 0){
				right = right - steeringPID.output;
				if( right < 0){ right = 1; }  //clip right channel to zero (one, actually)
			} else //if(steeringPID.input > 0)
			{
				left = left + steeringPID.output;
				if( left < 0){ left = 1; }  //clip right channel to zero (one, actually)
			}
			pidSetInputSameRuntime(0, left);
			pidSetInputSameRuntime(1, right);	
		}
    }*/

	gyroGetXYZ((unsigned char*)gdata);

	xlGetXYZ((unsigned char*)xldata);    // reads 6 bytes into xldata- bye aligned = int

/**** old *****
	xlGetXYZ((unsigned char*)acceldata);
	xl_data = xlReadXYZ();
*******/
	gyroWindow[windowIdx] = gdata[2];
	windowIdx = (windowIdx + 1) % GYRO_AVG_SAMPLES;

	gyro_accum = 0;
	for( i =0; i < GYRO_AVG_SAMPLES; i++){
		gyro_accum += gyroWindow[i];
	}
	gyroAvg = (gyro_accum - GYRO_AVG_SAMPLES*offsz) / GYRO_AVG_SAMPLES;

	//Update the setpoints
	if((currentMove->inputL != 0) && (currentMove->inputR != 0)){  
		//Only update steering controller if we are in motion
		steeringPID.error = steeringPID.input - gyroAvg;
       	UpdatePIDSteering(&steeringPID , gyroAvg);
		
		//left = pidObjs[0].input;    WRONG --> causes an implicit accumulation!!!
		//right = pidObjs[1].input; 
		left = currentMove->inputL;
		right = currentMove->inputR;
		
		// Depending on which way the bot is turning, choose which side to add correction to
		if( steeringPID.input <= 0){
			right = right - steeringPID.output;
			if( right < 0){ right = 1; }  //clip right channel to zero (one, actually)
		} else //if(steeringPID.input > 0)
		{
			left = left + steeringPID.output;
			if( left < 0){ left = 1; }  //clip right channel to zero (one, actually)
		}
		pidObjs[0].v_input = left;
		pidObjs[1].v_input = right;
	}

	// Section for saving telemetry data to flash
	if (TelemControl.onoff)
	{	if(t1_ticks >= TelemControl.start)
		{	if( telemSkip == 0)
			{	if(samplesToSave > 0)
				{samplesToSave--;
				  pidSaveTelemetry(); // save current sample
				  telemSkip = TelemControl.skip;  // reset skip
			  	 }
				else
				{TelemControl.onoff = 0;}	// turn off telemetry if no more counts
			}   
			else
			{ telemSkip--;}  
            }            
       }     

    _T5IF = 0;

}

// record current state
void pidSaveTelemetry(void)
{	telemU data;
//Stopwatch was already started in the cmdSpecialTelemetry function

	//Stopwatch was already started in the cmdSpecialTelemetry function
			data.telemStruct.timeStamp = (long)swatchTic(); 
// since T1 has higher priority, this might get interrupted 
	CRITICAL_SECTION_START
/*
			data.telemStruct.inputL = pidObjs[0].v_input;  
			data.telemStruct.inputR = pidObjs[1].v_input;
*/
//  save hall effect position instead of commanded thrust
		data.telemStruct.inputL = (int) (motor_count[0] & 0xffff);
		data.telemStruct.inputR = (int) (motor_count[1] & 0xffff);


	// save output instead of reading PWM (sync issue?)
			data.telemStruct.dcL = pidObjs[0].output;	// left
			data.telemStruct.dcR = pidObjs[1].output;	// right
			data.telemStruct.bemfL = bemf[0];
			data.telemStruct.bemfR = bemf[1];
	CRITICAL_SECTION_END
    			data.telemStruct.gyroX = gdata[0] - offsx;
			data.telemStruct.gyroY = gdata[1] - offsy;
			data.telemStruct.gyroZ = gdata[2] - offsz;
			data.telemStruct.gyroAvg = gyroAvg;
/* buggy only gets lower 8 bits 
			data.telemStruct.accelX = (int)(*(xl_data));
			data.telemStruct.accelY = (int)(*(xl_data+2));
			data.telemStruct.accelZ = (int)(*(xl_data+4));
*/
			data.telemStruct.accelX = xldata[0];
			data.telemStruct.accelY = xldata[1];
			data.telemStruct.accelZ = xldata[2];
			data.telemStruct.sOut = steeringPID.output;
			saveTelemData(&data); 
	
}

void setSteeringAngRate(int angRate)
{  
	_T5IE = 0;
	steeringPID.input = angRate;
	steeringPID.p = 0;
	steeringPID.i = 0;
	steeringPID.d = 0;
	steeringPID.iState = 0;
	_T5IE = 1;
}

//I need a better solution than this
void UpdatePIDSteering(pidT *pid, int y)
{
    pid->p = (long)pid->Kp * pid->error;
    pid->i = (long)pid->Ki * pid->iState;
    //Filtered derivative action applied directly to measurement
    pid->d = ((long)pid->Kd * (long)pid->d * (long)STEERING_GAIN_SCALER) / ((long)pid->Kd + (long)pid->Kp * (long)pid->N) -
        ((long)pid->Kd * (long)pid->Kp * (long)pid->N * ((long)y - (long)pid->y_old)) /
        ((long)pid->Kd + (long)pid->Kp * (long)pid->N);

    pid->preSat = (pid->p + pid->i + pid->d) / (long)STEERING_GAIN_SCALER ;

    if (pid->preSat > SATVAL)
    {
        pid->output = SATVAL;
    }else
    {
        pid->output = pid->preSat;
    }

    pid->iState += (long)(pid->error) + ((long)(pid->Kaw) * 
			((long)(pid->output) - (long)(pid->preSat)))/(long)GAIN_SCALER;
    pid->y_old = y;
}

void steeringSetGains(int Kp, int Ki, int Kd, int Kaw, int ff){
    steeringPID.Kp  = Kp;
    steeringPID.Ki  = Ki;
    steeringPID.Kd  = Kd;
    steeringPID.Kaw = Kaw;
	steeringPID.feedforward = ff;
}

/*
void getSteeringTelem(unsigned char* ptr){
	steeringPID.p;
	
}
*/

void setSampleSaveCount(int count){
	samplesToSave = count;
}
