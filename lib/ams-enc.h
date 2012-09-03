/*
 * Copyright (c) 2010, Regents of the University of California
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the University of California, Berkeley nor the names
 *   of its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
 
#define ENC_ADDR_R_RD 0b10000011		//A1 on AS5048B Pulled High, A2 Low
#define ENC_ADDR_R_WR 0b10000010

#define ENC_ADDR_L_RD 0b10000101		//A2 on AS5048B Pulled High, A1 Low
#define ENC_ADDR_L_WR 0b10000100

#define ENC_ADDR_AUX1_RD 0b10000001		//A1, A2 = low
#define ENC_ADDR_AUX1_WR 0b10000000

#define ENC_REG_ANGLE_HI 0xFE //angle comes in two bytes
#define ENC_REG_ANGLE_LOW 0xFF

#define ENC_REG_DIAG 0xFB

#define ENC_DIAG_COMP_HI 	0b00001000
#define ENC_DIAG_COMP_LOW 	0b00000100
#define ENC_DIAG_COF 		0b00000010
#define ENC_DIAG_OCF 		0b00000001

#define LSB2ENCDEG 0.0219
//#define ZEROANGLE 211.0
#define ZEROANGLE 338.1


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

/*****************************************************************************
* Function Name : encGetAux1PosRaw
* Description   : Read the angular position of the 1st auxiliary encoder, returns an int
* Parameters    : None
* Return Value  : None
*****************************************************************************/
int encGetAux1PosRaw(void);

//NK
unsigned char encGetDiagnostics(void);



