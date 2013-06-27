// Contents of this file are copyright Andrew Pullin, 2013

/******************************************************************************
* Name: settings.h
* Desc: Constants used by Andrew P. are included here.
* Author: pullin
******************************************************************************/
#ifndef __SETTINGS_H
#define __SETTINGS_H


//#warning "REQUIRED: Review and set radio channel & network parameters in firmware/source/settings.h  , then comment out this line."
/////// Radio settings ///////
#define RADIO_CHANNEL		0x19
//#warning "You have changed the radio channel from 0x0E to something else"
#define RADIO_SRC_ADDR 		0x2052
#define RADIO_PAN_ID  	0x2050
//Hard-coded destination address, must match basestation or XBee addr
#define RADIO_DST_ADDR		0x2051

#define RADIO_RXPQ_MAX_SIZE 	20
#define RADIO_TXPQ_MAX_SIZE	60

/////// System Service settings ///////
#define SYS_SERVICE_T1 // For legCtrl, hall
#define SYS_SERVICE_T2 // For hall, 400 Hz tick counter
#define SYS_SERVICE_T4 // For IMU, 300Hz
#define SYS_SERVICE_T5 // For steering, telemetry

/////// Configuration options ///////
//Configure project-wide for Hall Sensor operation
//#define HALL_SENSORS

#define TELEM_TYPE orTelemStruct_t
#define TELEM_INCLUDE "or_telem.h"
#define TELEMPACKFUNC(x) orTelemGetData(x)

#endif //__SETTINGS_H
