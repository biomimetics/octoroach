#include "pid.h"
#include "timer.h"
#include "gyro.h"
#include "steering.h"
#include "stopwatch.h"
#include "dfmem_extra.h"
#include "move_queue.h"
#include "xl.h"

#define TIMER_FREQUENCY     200                 // 400 Hz
#define TIMER_PERIOD        1/TIMER_FREQUENCY
#define SKIP_COUNT          2

long gyro_accum;
int j;
int gdata[3];
int xldata[3];
pidT steeringPID;
int steeringIsOn;
int gyroAvg;
int telemSkip;

int datactr;

int samplesToSave;

static int gyroWindow[GYRO_AVG_SAMPLES];

int windowIdx;

extern int offsx, offsy, offsz;

extern pidT pidObjs[NUM_PIDS];
extern int bemf[NUM_PIDS];

extern moveCmdT currentMove;

extern unsigned long t1_ticks; //needed to calculate new runtimes

void steeringSetup(void) {

    gyro_accum = 0;
    j = 0;

    initPIDObj(&steeringPID, STEERING_KP, STEERING_KI, STEERING_KD, STEERING_KAW, 0);
    setSteeringAngRate(0);

    // period value = Fcy/(prescale*Ftimer)
    unsigned int con_reg, period;

    // prescale 1:64
    con_reg = T5_ON & T5_IDLE_CON & T5_GATE_OFF & T5_PS_1_64 & T5_SOURCE_INT;
    // Period is set so that period = 5ms (200Hz), MIPS = 40
    period = 2083; // ~300Hz

    OpenTimer5(con_reg, period);
    ConfigIntTimer5(T5_INT_PRIOR_5 & T5_INT_ON);

    //offs = (float*)(gyroGetCalibParam());
	steeringIsOn = 1;
	windowIdx = 0;
}

void __attribute__((interrupt, no_auto_psv)) _T5Interrupt(void) {
    
    //int gyroAvg;
	int left, right;
	int i;

	gyroGetXYZ((unsigned char*)gdata);
	xlGetXYZ((unsigned char*)xldata);

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
		pidObjs[0].input = left;
		pidObjs[1].input = right;
	}

	// Section for saving telemetry data to flash
	// Uses telemSkip as a divisor to T5.
	telemU data;
	
	if( telemSkip == 0){
		if( samplesToSave > 0)
		{
			//Stopwatch was already started in the cmdSpecialTelemetry function
			data.telemStruct.timeStamp = (long)swatchTic(); 
			data.telemStruct.inputL = pidObjs[0].input;  
			data.telemStruct.inputR = pidObjs[1].input;
			data.telemStruct.dcL = PDC1;
			data.telemStruct.dcR = PDC2;
			data.telemStruct.gyroX = gdata[0] - offsx;
			data.telemStruct.gyroY = gdata[1] - offsy;
			data.telemStruct.gyroZ = gdata[2] - offsz;
			data.telemStruct.gyroAvg = gyroAvg;
			data.telemStruct.accelX = xldata[0];
			data.telemStruct.accelY = xldata[1];
			data.telemStruct.accelZ = xldata[2];
			data.telemStruct.bemfL = bemf[0];
			data.telemStruct.bemfR = bemf[1];
			data.telemStruct.sOut = steeringPID.output;
			samplesToSave--;
			saveTelemData(&data); 
		}
	}
	telemSkip = (telemSkip + 1) & ~SKIP_COUNT;
	//telemSkip = 0;
    _T5IF = 0;

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
