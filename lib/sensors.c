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
 * Sensor Block
 *
 * by Stanley S. Baek
 *
 * v. alpha
 *
 * Revisions:
 *  Stanley S. Baek      2010-08-27    Initial release
 *                      
 * 
 */


#include "sensors.h"
#include "timer.h"
#include "sclock.h"
#include "utils.h"
#include "flashmem.h"
#include "xl.h"
#include "gyro.h"
#include "dfmem.h"
#include "led.h"
#include <math.h>
#include "payload.h"
#include "cmd_const.h"
#include "radio.h"

#define TIMER_FREQUENCY     800                 // 800 Hz
#define TIMER_PERIOD        1/TIMER_FREQUENCY


/*-----------------------------------------------------------------------------
 *          Static Variables
-----------------------------------------------------------------------------*/
static unsigned char save_imu_data_ = 0;
static unsigned char estimate_pose_ = 0;
static unsigned char save_pose_ = 0;

static union {
    struct {
        unsigned int page;
        unsigned int byte;
    } index;
    unsigned char chr_index[4]; 

} MemLoc;

// Euler Angles (Euler 321)
static union {
    struct {
        float phi;      // roll
        float theta;    // pitch
        float psi;      // yaw
    } pose;
    float   pose_array[3];
    unsigned char chr_pose[12];
} Euler;


/*-----------------------------------------------------------------------------
 *      Declaration of static functions (Transceiver-specific functions)
-----------------------------------------------------------------------------*/
static void senEstimatePose(void);
static void senSavePose(void);
static void senSaveImuData(void);

/*-----------------------------------------------------------------------------
 *          Public functions
-----------------------------------------------------------------------------*/

void senSetup(void) {

    // period value = Fcy/(prescale*Ftimer)
    unsigned int con_reg, period;

    // prescale 1:8
    con_reg = T3_ON & T3_IDLE_CON & T3_GATE_OFF & T3_PS_1_8 & T3_SOURCE_INT;
    // Period is set so that period = 1.25ms (800Hz), MIPS = 40
    period = 6250; // 800Hz

    //con_reg = T3_ON & T3_IDLE_STOP & T3_GATE_OFF & T3_PS_1_64 & T3_SOURCE_INT;
    // Period is set so that period = 1.25ms (800Hz), MIPS = 40
    //period = 6250; // 100Hz

    OpenTimer3(con_reg, period);

    // interrupt for sensing
    ConfigIntTimer3(T3_INT_PRIOR_4 & T3_INT_ON);
    //ConfigIntTimer3(T3_INT_PRIOR_4 & T3_INT_OFF);
    
    MemLoc.index.page = FLASH_IMU_PAGE_START_LOC;
    MemLoc.index.byte = FLASH_IMU_BYTE_START_LOC;

    // initial values of roll, pitch, and yaw.
    Euler.pose.phi = 0.0;
    Euler.pose.theta = 0.0;
    Euler.pose.psi = 0.0;


}

float senGetPitch(void) {
    return Euler.pose.theta;
}

float senGetRoll(void) {
    return Euler.pose.phi;
}

float senGetYaw(void) {
    return Euler.pose.psi;
}

float* senGetPose(void) {
    return Euler.pose_array;
}



void senSetPoseSaveFlash(unsigned char flag) {
    save_pose_ = flag;
}

void senSetEstimatePose(unsigned char flag) {
    estimate_pose_ = flag;
}

void senSetImuDataSave(unsigned char flag) {
    save_imu_data_ = flag;
}

void senGetMemLocIndex(unsigned int *page, unsigned int *byte) {
    *page = MemLoc.index.page;
    *byte = MemLoc.index.byte;
}

void senGetIMUData(unsigned char status, unsigned char type){
	
    unsigned char *xl_data, *gyro_data;
    Payload pld;
    
    //If loop is running, add timing info to data.
    //if (type == CMD_GET_IMU_LOOP)
    //{
    //    pld = payCreateEmpty(16);
    //    samp_num = (unsigned char*)&g_samples;
    //    payAppendData(pld, 12, 4, samp_num);
    //}
    //else
    //{
        pld = payCreateEmpty(12);
    //}
    
	paySetStatus(pld,status);
	paySetType(pld,type);
    
    xl_data = xlReadXYZ();
	payAppendData(pld, 0, 6, xl_data);
    gyro_data = gyroReadXYZ();
	payAppendData(pld, 6, 6, gyro_data);
    
    radioSendPayload(macGetDestAddr(), pld);
}


/*-----------------------------------------------------------------------------
 * ----------------------------------------------------------------------------
 * The functions below are intended for internal use, i.e., private methods.
 * Users are recommended to use functions defined above.
 * ----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

/*****************************************************************************
* Function Name : _T3Interrupt
* Description   : Interrupt hander for Timer 3
* Parameters    : None
* Return Value  : None
*****************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) {

    if (save_imu_data_) {
        senSaveImuData();
    }

    if (estimate_pose_) {
        senEstimatePose();
        if (save_pose_) senSavePose();
    }

    _T3IF = 0;

}


static void senEstimatePose(void) {

    float dphi, dtheta, dpsi;
    float rate[3];
    float wx, wy, wz;

    gyroReadXYZ();
    gyroGetRadXYZ(rate);

    // coordinate conversion
    wx = rate[0];
    wy = -rate[2];
    wz = rate[1];

    dphi = wx + wy*sin(Euler.pose.phi)*tan(Euler.pose.theta) + 
           wz*cos(Euler.pose.phi)*tan(Euler.pose.theta);
    dtheta = wy*cos(Euler.pose.phi) - wz*sin(Euler.pose.phi);
    dpsi = wy*sin(Euler.pose.phi)/cos(Euler.pose.theta) + 
           wz*cos(Euler.pose.phi)/cos(Euler.pose.theta);


    Euler.pose.phi += dphi*TIMER_PERIOD;
    Euler.pose.theta += dtheta*TIMER_PERIOD;
    Euler.pose.psi += dpsi*TIMER_PERIOD;

}

static void senSavePose(void) {

    unsigned long tic;
    unsigned char *tic_char;
    
    static int count = 0;
    static unsigned char buf_index = 1;

    if (count == 100) {
        LED_BLUE = ~LED_BLUE;
        count = 0;
    }
    count++;

    tic_char = (unsigned char*)&tic;
    tic = sclockGetTime();

    // save imu data to the memory.
    dfmemWriteBuffer(tic_char, 4, MemLoc.index.byte, buf_index); 
    MemLoc.index.byte += 4;
    dfmemWriteBuffer(Euler.chr_pose, 12, MemLoc.index.byte, buf_index); 
    MemLoc.index.byte += 12;    

    if (MemLoc.index.byte >= 512) {
        dfmemWriteBuffer2MemoryNoErase(MemLoc.index.page++, buf_index);
        MemLoc.index.byte = 0;
        buf_index ^= 0x01;
    }
}



static void senSaveImuData(void) {

    unsigned char *xl_data, *gyro_data;
    unsigned char i;
    unsigned long tic;
    unsigned char *tic_char;
    unsigned char imudata[16];
    
    static int count = 0;
    static unsigned char buf_index = 1;

    if (count == 100) {
        LED_BLUE = ~LED_BLUE;
        count = 0;
    }
    count++;

    tic_char = (unsigned char*)&tic;
    tic = sclockGetTime();
    xl_data = xlReadXYZ();
    gyro_data = gyroReadXYZ();

    for (i = 0; i < 4; ++i) {
        imudata[i] = tic_char[i];
    }

    for(i = 0; i < 6; ++i) {
        imudata[i+4] = xl_data[i];
        imudata[i+10] = gyro_data[i];
    }

    // save imu data to the memory.
    
    dfmemWriteBuffer(imudata, 16, MemLoc.index.byte, buf_index); 
   
    MemLoc.index.byte += 16;
    if (MemLoc.index.byte >= 512) {
        dfmemWriteBuffer2MemoryNoErase(MemLoc.index.page++, buf_index);
        MemLoc.index.byte = 0;
        buf_index ^= 0x01;
    }
    
}




