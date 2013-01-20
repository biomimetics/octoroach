// Contents of this file are copyright Andrew Pullin, 2013

/*
 *
 * This module implements the 'Emergency Stop' function for the OctoRoach robot
 *
 * by A. Pullin
 *
 * Notes:
 *  - This code is specific to the OctoRoach robot. Other robots may need
 *    different EStop functionality. PWM module is switched off, and all
 *    duty cycles are zero'd.
 *    This is not intended to put the robot into a recoverable state.
 *
 * Usage:
 *  EmergencyStop();
 * 	//Reset robot by sending cmdSoftwareReset(...)
 *  
 */

#include "pwm.h"

/*****************************************************************************
* Function Name : EmergencyStop
* Description   : EStop function for OctoRoach. Halts motor control.
* Parameters    : None
* Return Value  : None
*****************************************************************************/
void EmergencyStop(){
	PTCON &= PWM_DIS;  //Disable PWM output
	PDC1 = 0;	//Zero all duty cycles
	PDC2 = 0;
	PDC3 = 0;
	PDC4 = 0;
}
