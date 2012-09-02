/* 
 * File:   imu.h
 * Author: kohut
 *
 * Created on August 15, 2012, 3:37 PM
 */

#ifndef IMU_H
#define	IMU_H

void imuSetup(); // to be called in main

int imuGetGyroXValue();
int imuGetGyroYValue();
int imuGetGyroZValue();

float imuGetGyroXValueDeg();
float imuGetGyroYValueDeg();
float imuGetGyroZValueDeg();

int imuGetGyroZValueAvg();
float imuGetGyroZValueAvgDeg();

float imuGetBodyZPositionDeg();

void imuTest(float* ptr);

//Constants
#define GYRO_DRIFT_THRESH 5
#define LSB2DEG    0.0695652174

#endif

