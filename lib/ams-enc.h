// Modifications and additions to this file made by Andrew Pullin are copyright, 2013
// Copyrights are acknowledged for portions of this code extant before modifications by Andrew Pullin 
// Any application of BSD or other license to copyright content without the authors express approval
// is invalid and void.

/*
 *
 *
 * Austria Microsystems AS5048B magnetic encoder I2C Interface
 *
 * by Duncan Haldane
 *
 * v.1.0
 *
 * Revisions:
 *  Duncan Haldane      2012-05-15    Initial release
 *                      
 * Notes:
 *  - This module uses the I2C2 port for communicating with the AMS encoder chips
 *Usage:
 * #include "enc.h"
 *
 *  // initialize encoder module
 *  encSetup();
 * 
 * //get position of right encoder write to ENCPOS struct
 * void encGetRPos()
 *
 *
 *
 */
 
 /*****************************************************************************
* Function Name : encSetup
* Description   : Initialize encoder
* Parameters    : None
* Return Value  : None
*****************************************************************************/
 void encSetup(void);
 
 /*****************************************************************************
* Function Name : encGetRPos
* Description   : Read the angular position of the right encoder, write to struct encPos
* Parameters    : None
* Return Value  : None
*****************************************************************************/
void encGetRPos();

/*****************************************************************************
* Function Name : encGetLPos
* Description   : Read the angular position of the left encoder, write to struct encPos
* Parameters    : None
* Return Value  : None
*****************************************************************************/
void encGetLPos();

/*****************************************************************************
* Function Name : encGetAux1Pos
* Description   : Read the angular position of the 1st auxiliary encoder
* Parameters    : None
* Return Value  : None
*****************************************************************************/
float encGetAux1Pos(void);