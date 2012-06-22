/******************************************************************************
* Name: pullin_const.h
* Desc: Constants used by Andrew P. are included here.
* Date: 2010-09-22
* Author: pullin
******************************************************************************/
#ifndef __PULLIN_CONST_H
#define __PULLIN_CONST_H

#include "generic_typedefs.h"

//Define constants used for radio init
#define MY_CHAN			0x19
//#warning "You have changed the radio channel from 0x19 to something else"
#define SRC_PAN_ID 		0x2050
#define SRC_ADDR 		0x2052
//Hard code in destination address for now, update to be dynamic later
#define DST_ADDR		0x2051

// Radio queue sizes
#define RXPQ_MAX_SIZE 	16
#define TXPQ_MAX_SIZE	16

//sysService enabling defines
#define SYS_SERVICE_T1 // For legCtrl, hall
#define SYS_SERVICE_T2 // For hall, 400 Hz tick counter
#define SYS_SERVICE_T5 // For steering, telemetry

//Configure project-wide for Hall Sensor operation
//#define HALL_SENSORS

#endif //__PULLIN_CONST_H
