/******************************************************************************
* Name: or_const.h
* Desc: Constants used for the OctoRoACH robot are listed here.
* Date: 2010-09-22
* Author: pullin
******************************************************************************/
#ifndef __OR_CONST_H
#define __OR_CONST_H

#include "generic_typedefs.h"

//#error "REQUIRED: Review and set radio channel & network parameters in lib/or_const.h , then comment out this line."

// Motile address ; all MRI robots were shipped with this configuration
#define SRC_ADDR 		0x3002
#define SRC_PAN_ID 		0x3000
#define MY_CHAN			0x0e
//Hard code in destination address for now, update to be dynamic later
#define DST_ADDR		0x3001


//Define constants used for radio init
/* */
//#define SRC_ADDR 		0x2052
//#define SRC_PAN_ID 		0x2050
//#define MY_CHAN			0x19
//Hard code in destination address for now, update to be dynamic later
//#define DST_ADDR		0x2051
/* */

#define RXPQ_MAX_SIZE 	50
#define TXPQ_MAX_SIZE	50

#endif //__OR_CONST_H
