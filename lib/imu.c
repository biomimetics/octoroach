#include "utils.h"
#include "payload.h"
#include "led.h"
#include "gyro.h"
#include "xl.h"
#include "pid.h"
#include "dfilter_avg.h"
#include "adc_pid.h"
#include "leg_ctrl.h"
#include "sys_service.h"
#include "ams-enc.h"
#include "imu.h"

#define TIMER_FREQUENCY     300                 // 300 Hz
#define TIMER_PERIOD        1/TIMER_FREQUENCY   //This is used for numerical integration

//Setup for Gyro Z averaging filter
#define GYRO_AVG_SAMPLES 	8


//Filter stuctures for gyro variables
static dfilterAvgInt_t gyroZavg;


//TODO: change these to arrays
static int lastGyroXValue = 0;
static int lastGyroYValue = 0;
static int lastGyroZValue = 0;

static float lastGyroXValueDeg = 0.0;
static float lastGyroYValueDeg = 0.0;
static float lastGyroZValueDeg = 0.0;

static int lastGyroZValueAvg = 0;

static float lastGyroZValueAvgDeg = 0.0;

static float lastBodyZPositionDeg = 0.0;

static void SetupTimer4(); 
static void imuServiceRoutine(void);  //To be installed with sysService
//The following local functions are called by the service routine:
static void imuISRHandler(void);

////   Private functions
////////////////////////
/////////        IMU ISR          ////////
////////  Installed to Timer4 @ 300hz  ////////

static void imuServiceRoutine(void){
    //This intermediate function is used in case we want to tie other
    //sub-taks to the imu service routine.
    //TODO: Is this neccesary?
    imuISRHandler();
}

static void imuISRHandler(){
	
	int gyroData[3];

	/////// Get Gyro data and calc average via filter
        gyroReadXYZ(); //bad design of gyro module; todo: humhu
	gyroGetIntXYZ(gyroData);
	

        lastGyroXValue = gyroData[0];
        lastGyroYValue = gyroData[1];
        lastGyroZValue = gyroData[2];

        //Threshold:
        
        if((lastGyroXValue < 8) && (lastGyroXValue > -8)){
            lastGyroXValue = lastGyroXValue >> 1; //fast divide by 2
        }
        if((lastGyroYValue < 8) && (lastGyroYValue > -8)){
            lastGyroYValue = lastGyroYValue >> 1; //fast divide by 2
        }
        if((lastGyroZValue < 8) && (lastGyroZValue > -8)){
            lastGyroZValue = lastGyroZValue >> 1; //fast divide by 2
        }
        

        lastGyroXValueDeg = (float) (lastGyroXValue*LSB2DEG);
        lastGyroYValueDeg = (float) (lastGyroYValue*LSB2DEG);
        lastGyroZValueDeg = (float) (lastGyroZValue*LSB2DEG); 

        dfilterAvgUpdate(&gyroZavg, gyroData[2]);

        lastGyroZValueAvg = dfilterAvgCalc(&gyroZavg);

        lastGyroZValueAvgDeg = (float)lastGyroZValueAvg*LSB2DEG;

        lastBodyZPositionDeg = lastBodyZPositionDeg + lastGyroZValueDeg*TIMER_PERIOD;
}

static void SetupTimer4(){
    ///// Timer 4 setup, 300Hz /////
    // period value = Fcy/(prescale*Ftimer)
    unsigned int T4CON1value, T4PERvalue;
    // prescale 1:64
    T4CON1value = T4_ON & T4_IDLE_CON & T4_GATE_OFF & T4_PS_1_64 & T4_SOURCE_INT;
    // Period is set so that period = 3.3ms (300Hz), MIPS = 40
    T4PERvalue = 2083; // ~300Hz (40e6/(64*2083) where 64 is the prescaler
    int retval;
    retval = sysServiceConfigT4(T4CON1value, T4PERvalue, T4_INT_PRIOR_6 & T4_INT_ON);
}

////   Public functions
////////////////////////
void imuSetup(){
    int retval;
    retval = sysServiceInstallT4(imuServiceRoutine);
    SetupTimer4();
    dfilterAvgCreate(&gyroZavg, GYRO_AVG_SAMPLES);
}

int imuGetGyroXValue() {
    return lastGyroXValue;
}

int imuGetGyroYValue() {
    return lastGyroYValue;
}

int imuGetGyroZValue() {
    return lastGyroZValue;
}


float imuGetGyroXValueDeg() {
    return lastGyroXValueDeg;
}

float imuGetGyroYValueDeg() {
    return lastGyroYValueDeg;
}

float imuGetGyroZValueDeg() {
    return lastGyroZValueDeg;
}


int imuGetGyroZValueAvg() {
    return lastGyroZValueAvg;
}

float imuGetGyroZValueAvgDeg() {
    return lastGyroZValueAvgDeg;
}


float imuGetBodyZPositionDeg() {
    return lastBodyZPositionDeg;
}

void imuResetGyroZAvg(){
    filterZero(&gyroZavg);
}
