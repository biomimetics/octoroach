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
#define SRC_ADDR 		0x2052
#define SRC_PAN_ID 		0x2050
#define RXPQ_MAX_SIZE 	100
#define TXPQ_MAX_SIZE	100
#define MY_CHAN			0x19
//#warning "You have changed the radio channel from 0x19 to something else"

//Hard code in destination address for now, update to be dynamic later
#define DST_ADDR		0x2051



#endif //__PULLIN_CONST_H
