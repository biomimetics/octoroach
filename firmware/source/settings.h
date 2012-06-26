/******************************************************************************
* Name: settings.h
* Desc: Constants used by Andrew P. are included here.
* Date: 2012-06-26
* Author: pullin
******************************************************************************/
#ifndef __SETTINGS_H
#define __SETTINGS_H


/////// Radio settings ///////
#define RADIO_CHANNEL			0x19
//#warning "You have changed the radio channel from 0x19 to something else"
#define RADIO_SRC_PAN_ID 		0x2050
#define RADIO_SRC_ADDR 		0x2052
//Hard code in destination address for now, update to be dynamic later
#define RADIO_DST_ADDR		0x2051

// Radio queue sizes
#define RADIO_RXPQ_MAX_SIZE 	16
#define RADIO_TXPQ_MAX_SIZE	16

/////// System Service settings ///////
#define SYS_SERVICE_T1 // For legCtrl, hall
#define SYS_SERVICE_T2 // For hall, 400 Hz tick counter
#define SYS_SERVICE_T5 // For steering, telemetry

/////// Configuration options ///////
//Configure project-wide for Hall Sensor operation
#define HALL_SENSORS

#endif //__SETTINGS_H
