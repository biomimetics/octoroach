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
 * PID
 *
 * by Kevin Peterson
 *
 * v.beta
 *
 * Revisions:
 *  Kevin Peterson      2012-04-05    Initial release
 *                      
 * Notes:
 */

#ifndef __PID_HW_H
#define __PID_HW_H

/*
typedef struct {
    tPID* p_PID;


} pidStruct;
*/

typedef tPID* pPID;

void pidCreate(pPID controller, fractional* abcCoefficients, fractional* controlHistory);
void pidSetFloatCoeffs(tPID* controller, float Kp, float Ki, float Kd);
//void pidSetFracCoeffs(tPID* controller, fractional Kp, fractional Ki, fractional Kd);
void pidSetFracCoeffs(unsigned int pid_num, fractional Kp, fractional Ki, fractional Kd);
void pidSetReference(tPID* controller, fractional reference);
fractional pidRun(tPID* controller, fractional feedback);
void dspPIDSetup();

#endif //__PID_HW_H

