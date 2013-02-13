// Contents of this file are copyright Andrew Pullin, 2013

//or_telem.c , OctoRoACH specific telemetry packet format


#include "or_telem.h"

#include <xc.h>
#include "pid.h"
#include "gyro.h"
#include "xl.h"
#include "ams-enc.h"
#include "imu.h"
#include "leg_ctrl.h"
#include "tail_ctrl.h"
#include "adc_pid.h"
#include "tail_ctrl.h"

// TODO (apullin) : Remove externs by adding getters to other modules
extern pidObj motor_pidObjs[NUM_MOTOR_PIDS];
extern int bemf[NUM_MOTOR_PIDS];
extern pidObj steeringPID;
extern pidObj tailPID;

void orTelemGetData(unsigned char* ptr) {
    /////// Get XL data
    orTelemStruct_t* tptr;
    tptr = (orTelemStruct_t*) ptr;
    tptr->inputL = motor_pidObjs[0].input;
    tptr->inputR = motor_pidObjs[1].input;
    tptr->dcL = PDC1;
    tptr->dcR = PDC2;
    tptr->gyroX = imuGetGyroXValue();
    tptr->gyroY = imuGetGyroYValue();
    tptr->gyroZ = imuGetGyroZValue();
    tptr->gyroAvg = imuGetGyroZValueAvgDeg();
    tptr->accelX = 0;
    tptr->accelY = 0;
    tptr->accelZ = 0;
    tptr->accelX = 0;
    tptr->accelY = 0;
    tptr->accelZ = 0;
    tptr->bemfL = bemf[0];
    tptr->bemfR = bemf[1];
    tptr->Vbatt = adcGetVBatt();
    tptr->steerIn = steeringPID.input;
    tptr->steerOut = steeringPID.output;
    tptr->motor_count[0] = 0;
    tptr->motor_count[1] = 0;
    tptr->yawAngle = imuGetBodyZPositionDeg();
}

//This may be unneccesary, since the telemtry type isn't totally anonymous

unsigned int orTelemGetSize() {
    return sizeof (orTelemStruct_t);
}