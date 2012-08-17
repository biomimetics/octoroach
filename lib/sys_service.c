/*
 * Copyright (c) 2008-2012, Regents of the University of California
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
 * System Service multi-use timer abstraction
 *
 * by Andrew Pullin
 *
 * v.0.1 alpha
 * Notes:
 *  - Provides function vectors to be called for each timer.
 */

#include "sys_service.h"
#include "timer.h"

#define SERVICE_ENABLE         1
#define SERVICE_DISABLE        0

// Setup Function
void sysServiceSetup(){
//TODO: Decide what goes here
    _T1IE = 0;
    _T5IE = 0;
}

////////////////////     Timer 1     //////////////////
#ifdef SYS_SERVICE_T1

unsigned int installedIdxT1 = 0;
unsigned long sys_T1_ticks = 0;
static void (*serviceVectorT1[SERVICE_VECT_LEN])(void);
int serviceVectorT1Enabled[SERVICE_VECT_LEN];
static char T1_already_confgured = 0;
//Timer 1 Service ISR

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    int i;
    for(i=0; i<SERVICE_VECT_LEN; i++){
        if(serviceVectorT1[i] && serviceVectorT1Enabled[i] )
            serviceVectorT1[i]();
    }
    sys_T1_ticks++;
    //Clear Timer1 interrupt flag
    _T1IF = 0;
}

//Timer 1 install function
int sysServiceInstallT1(void* func){
    if(installedIdxT1 < SERVICE_VECT_LEN){
        serviceVectorT1[installedIdxT1] = func;
        serviceVectorT1Enabled[installedIdxT1] = SERVICE_ENABLE;
        installedIdxT1++;
        return (installedIdxT1-1); //succesful install
    }
    return -1; //Error, no more room
}

//Timer 1 setup function
int sysServiceConfigT1(unsigned int T1conval, unsigned int T1perval,
                        unsigned int T1intconval){
    //Todo: is there any way to have a compile time semaphore here?
    if(T1_already_confgured){
        return -1;
    } 
    else{
        T1_already_confgured = 1;
        OpenTimer1(T1conval, T1perval);
        ConfigIntTimer1(T1intconval);
        return 0;
    }
}

//T1 ticks getter
unsigned long getT1_ticks(){
    return sys_T1_ticks;
}

//Service enabler
int sysServiceEnableSvcT1(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorT1Enabled[svcNum] = SERVICE_ENABLE;
        return 0;
    }
    else{ return svcNum; }
}
//Service disabler
int sysServiceDisableSvcT1(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorT1Enabled[svcNum] = SERVICE_DISABLE;
        return 0;
    }
    else{ return svcNum; }
}
#endif
///////////////////////////////////////////////////////

////////////////////     Timer 2     //////////////////
#ifdef SYS_SERVICE_T2

unsigned int installedIdxT2 = 0;
unsigned long sys_T2_ticks = 0;
static void (*serviceVectorT2[SERVICE_VECT_LEN])(void);
int serviceVectorT2Enabled[SERVICE_VECT_LEN];
static char T2_already_confgured = 0;
//Timer 2 Service ISR

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void) {
    int i;
    for(i=0; i<SERVICE_VECT_LEN; i++){
        if(serviceVectorT2[i] && serviceVectorT2Enabled[i] )
            serviceVectorT2[i]();
    }
    sys_T2_ticks++;
    //Clear Timer2 interrupt flag
    _T2IF = 0;
}

//Timer 2 install function
int sysServiceInstallT2(void* func){
    if(installedIdxT2 < SERVICE_VECT_LEN){
        serviceVectorT2[installedIdxT2] = func;
        serviceVectorT2Enabled[installedIdxT2] = SERVICE_ENABLE;
        installedIdxT2++;
        return (installedIdxT2-1); //succesful install
    }
    return -1; //Error, no more room
}

//Timer 2 setup function
int sysServiceConfigT2(unsigned int T2conval, unsigned int T2perval,
                        unsigned int T2intconval){
    //Todo: is there any way to have a compile time semaphore here?
    if(T2_already_confgured){
        return -1;
    }
    else{
        T2_already_confgured = 1;
        OpenTimer2(T2conval, T2perval);
        ConfigIntTimer2(T2intconval);
        return 0;
    }
}

//T2 ticks getter
unsigned long getT2_ticks(){
    return sys_T2_ticks;
}

//Service enabler
int sysServiceEnableSvcT2(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorT2Enabled[svcNum] = SERVICE_ENABLE;
        return 0;
    }
    else{ return svcNum; }
}
//Service disabler
int sysServiceDisableSvcT2(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorT2Enabled[svcNum] = SERVICE_DISABLE;
        return 0;
    }
    else{ return svcNum; }
}
#endif
///////////////////////////////////////////////////////

////////////////////     Timer 3     //////////////////
#ifdef SYS_SERVICE_T3

unsigned int installedIdxT3 = 0;
unsigned long sys_T3_ticks = 0;
static void (*serviceVectorT3[SERVICE_VECT_LEN])(void);
int serviceVectorT3Enabled[SERVICE_VECT_LEN];
static char T3_already_confgured = 0;
//Timer 3 Service ISR

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) {
    int i;
    for(i=0; i<SERVICE_VECT_LEN; i++){
        if(serviceVectorT3[i] && serviceVectorT3Enabled[i] )
            serviceVectorT3[i]();
    }
    sys_T3_ticks++;
    //Clear Timer3 interrupt flag
    _T3IF = 0;
}

//Timer 3 install function
int sysServiceInstallT3(void* func){
    if(installedIdxT3 < SERVICE_VECT_LEN){
        serviceVectorT3[installedIdxT3] = func;
        serviceVectorT3Enabled[installedIdxT3] = SERVICE_ENABLE;
        installedIdxT3++;
        return (installedIdxT3-1); //succesful install
    }
    return -1; //Error, no more room
}

//Timer 3 setup function
int sysServiceConfigT3(unsigned int T3conval, unsigned int T3perval,
                        unsigned int T3intconval){
    //Todo: is there any way to have a compile time semaphore here?
    if(T3_already_confgured){
        return -1;
    }
    else{
        T3_already_confgured = 1;
        OpenTimer3(T3conval, T3perval);
        ConfigIntTimer3(T3intconval);
        return 0;
    }
}

//T3 ticks getter
unsigned long getT3_ticks(){
    return sys_T3_ticks;
}

//Service enabler
int sysServiceEnableSvcT3(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorT3Enabled[svcNum] = SERVICE_ENABLE;
        return 0;
    }
    else{ return svcNum; }
}
//Service disabler
int sysServiceDisableSvcT3(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorT3Enabled[svcNum] = SERVICE_DISABLE;
        return 0;
    }
    else{ return svcNum; }
}
#endif
///////////////////////////////////////////////////////

////////////////////     Timer 4     //////////////////
#ifdef SYS_SERVICE_T4

unsigned int installedIdxT4 = 0;
unsigned long sys_T4_ticks = 0;
static void (*serviceVectorT4[SERVICE_VECT_LEN])(void);
int serviceVectorT4Enabled[SERVICE_VECT_LEN];
static char T4_already_confgured = 0;
//Timer 4 Service ISR

void __attribute__((interrupt, no_auto_psv)) _T4Interrupt(void) {
    int i;
    for(i=0; i<SERVICE_VECT_LEN; i++){
        if(serviceVectorT4[i] && serviceVectorT4Enabled[i] )
            serviceVectorT4[i]();
    }
    sys_T4_ticks++;
    //Clear Timer4 interrupt flag
    _T4IF = 0;
}

//Timer 4 install function
int sysServiceInstallT4(void* func){
    if(installedIdxT4 < SERVICE_VECT_LEN){
        serviceVectorT4[installedIdxT4] = func;
        serviceVectorT4Enabled[installedIdxT4] = SERVICE_ENABLE;
        installedIdxT4++;
        return (installedIdxT4-1); //succesful install
    }
    return -1; //Error, no more room
}

//Timer 4 setup function
int sysServiceConfigT4(unsigned int T4conval, unsigned int T4perval,
                        unsigned int T4intconval){
    //Todo: is there any way to have a compile time semaphore here?
    if(T4_already_confgured){
        return -1;
    }
    else{
        T4_already_confgured = 1;
        OpenTimer4(T4conval, T4perval);
        ConfigIntTimer4(T4intconval);
        return 0;
    }
}

//T4 ticks getter
unsigned long getT4_ticks(){
    return sys_T4_ticks;
}

//Service enabler
int sysServiceEnableSvcT4(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorT4Enabled[svcNum] = SERVICE_ENABLE;
        return 0;
    }
    else{ return svcNum; }
}
//Service disabler
int sysServiceDisableSvcT4(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorT4Enabled[svcNum] = SERVICE_DISABLE;
        return 0;
    }
    else{ return svcNum; }
}
#endif
///////////////////////////////////////////////////////

////////////////////     Timer 5     //////////////////
#ifdef SYS_SERVICE_T5

unsigned int installedIdxT5 = 0;
unsigned long sys_T5_ticks = 0;
static void (*serviceVectorT5[SERVICE_VECT_LEN])(void);
int serviceVectorT5Enabled[SERVICE_VECT_LEN];
static char T5_already_confgured = 0;
//Timer 5 Service ISR

void __attribute__((interrupt, no_auto_psv)) _T5Interrupt(void) {
    int i;
    for(i=0; i<SERVICE_VECT_LEN; i++){
        if(serviceVectorT5[i] && serviceVectorT5Enabled[i] )
            serviceVectorT5[i]();
    }
    sys_T5_ticks++;
    //Clear Timer5 interrupt flag
    _T5IF = 0;
}

//Timer 5 install function
int sysServiceInstallT5(void* func){
    if(installedIdxT5 < SERVICE_VECT_LEN){
        serviceVectorT5[installedIdxT5] = func;
        serviceVectorT5Enabled[installedIdxT5] = SERVICE_ENABLE;
        installedIdxT5++;
        return (installedIdxT5-1); //succesful install
    }
    return -1; //Error, no more room
}

//Timer 5 setup function
int sysServiceConfigT5(unsigned int T5conval, unsigned int T5perval,
                        unsigned int T5intconval){
    //Todo: is there any way to have a compile time semaphore here?
    if(T5_already_confgured){
        return -1;
    }
    else{
        T5_already_confgured = 1;
        OpenTimer5(T5conval, T5perval);
        ConfigIntTimer5(T5intconval);
        return 0;
    }
}

//T5 ticks getter
unsigned long getT5_ticks(){
    return sys_T5_ticks;
}

//Service enabler
int sysServiceEnableSvcT5(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorT5Enabled[svcNum] = SERVICE_ENABLE;
        return 0;
    }
    else{ return svcNum; }
}
//Service disabler
int sysServiceDisableSvcT5(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorT5Enabled[svcNum] = SERVICE_DISABLE;
        return 0;
    }
    else{ return svcNum; }
}
#endif
///////////////////////////////////////////////////////

////////////////////     Timer 6     //////////////////
#ifdef SYS_SERVICE_T6

unsigned int installedIdxT6 = 0;
unsigned long sys_T6_ticks = 0;
static void (*serviceVectorT6[SERVICE_VECT_LEN])(void);
int serviceVectorT6Enabled[SERVICE_VECT_LEN];
static char T6_already_confgured = 0;
//Timer 6 Service ISR

void __attribute__((interrupt, no_auto_psv)) _T6Interrupt(void) {
    int i;
    for(i=0; i<SERVICE_VECT_LEN; i++){
        if(serviceVectorT6[i] && serviceVectorT6Enabled[i] )
            serviceVectorT6[i]();
    }
    sys_T6_ticks++;
    //Clear Timer6 interrupt flag
    _T6IF = 0;
}

//Timer 6 install function
int sysServiceInstallT6(void* func){
    if(installedIdxT6 < SERVICE_VECT_LEN){
        serviceVectorT6[installedIdxT6] = func;
        serviceVectorT6Enabled[installedIdxT6] = SERVICE_ENABLE;
        installedIdxT6++;
        return (installedIdxT6-1); //succesful install
    }
    return -1; //Error, no more room
}

//Timer 6 setup function
int sysServiceConfigT6(unsigned int T6conval, unsigned int T6perval,
                        unsigned int T6intconval){
    //Todo: is there any way to have a compile time semaphore here?
    if(T6_already_confgured){
        return -1;
    }
    else{
        T6_already_confgured = 1;
        OpenTimer6(T6conval, T6perval);
        ConfigIntTimer6(T6intconval);
        return 0;
    }
}

//T6 ticks getter
unsigned long getT6_ticks(){
    return sys_T6_ticks;
}

//Service enabler
int sysServiceEnableSvcT6(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorT6Enabled[svcNum] = SERVICE_ENABLE;
        return 0;
    }
    else{ return svcNum; }
}
//Service disabler
int sysServiceDisableSvcT6(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorT6Enabled[svcNum] = SERVICE_DISABLE;
        return 0;
    }
    else{ return svcNum; }
}
#endif
///////////////////////////////////////////////////////

////////////////////     Timer 7     //////////////////
#ifdef SYS_SERVICE_T7

unsigned int installedIdxT7 = 0;
unsigned long sys_T7_ticks = 0;
static void (*serviceVectorT7[SERVICE_VECT_LEN])(void);
int serviceVectorT7Enabled[SERVICE_VECT_LEN];
static char T7_already_confgured = 0;
//Timer 7 Service ISR

void __attribute__((interrupt, no_auto_psv)) _T7Interrupt(void) {
    int i;
    for(i=0; i<SERVICE_VECT_LEN; i++){
        if(serviceVectorT7[i] && serviceVectorT7Enabled[i] )
            serviceVectorT7[i]();
    }
    sys_T7_ticks++;
    //Clear Timer7 interrupt flag
    _T7IF = 0;
}

//Timer 7 install function
int sysServiceInstallT7(void* func){
    if(installedIdxT7 < SERVICE_VECT_LEN){
        serviceVectorT7[installedIdxT7] = func;
        serviceVectorT7Enabled[installedIdxT7] = SERVICE_ENABLE;
        installedIdxT7++;
        return (installedIdxT7-1); //succesful install
    }
    return -1; //Error, no more room
}

//Timer 7 setup function
int sysServiceConfigT7(unsigned int T7conval, unsigned int T7perval,
                        unsigned int T7intconval){
    //Todo: is there any way to have a compile time semaphore here?
    if(T7_already_confgured){
        return -1;
    }
    else{
        T7_already_confgured = 1;
        OpenTimer7(T7conval, T7perval);
        ConfigIntTimer7(T7intconval);
        return 0;
    }
}

//T7 ticks getter
unsigned long getT7_ticks(){
    return sys_T7_ticks;
}

//Service enabler
int sysServiceEnableSvcT7(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorT7Enabled[svcNum] = SERVICE_ENABLE;
        return 0;
    }
    else{ return svcNum; }
}
//Service disabler
int sysServiceDisableSvcT7(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorT7Enabled[svcNum] = SERVICE_DISABLE;
        return 0;
    }
    else{ return svcNum; }
}
#endif
///////////////////////////////////////////////////////

////////////////////     Timer 8     //////////////////
#ifdef SYS_SERVICE_T8

unsigned int installedIdxT8 = 0;
unsigned long sys_T8_ticks = 0;
static void (*serviceVectorT8[SERVICE_VECT_LEN])(void);
int serviceVectorT8Enabled[SERVICE_VECT_LEN];
static char T8_already_confgured = 0;
//Timer 8 Service ISR

void __attribute__((interrupt, no_auto_psv)) _T8Interrupt(void) {
    int i;
    for(i=0; i<SERVICE_VECT_LEN; i++){
        if(serviceVectorT8[i] && serviceVectorT8Enabled[i] )
            serviceVectorT8[i]();
    }
    sys_T8_ticks++;
    //Clear Timer8 interrupt flag
    _T8IF = 0;
}

//Timer 8 install function
int sysServiceInstallT8(void* func){
    if(installedIdxT8 < SERVICE_VECT_LEN){
        serviceVectorT8[installedIdxT8] = func;
        serviceVectorT8Enabled[installedIdxT8] = SERVICE_ENABLE;
        installedIdxT8++;
        return (installedIdxT8-1); //succesful install
    }
    return -1; //Error, no more room
}

//Timer 8 setup function
int sysServiceConfigT8(unsigned int T8conval, unsigned int T8perval,
                        unsigned int T8intconval){
    //Todo: is there any way to have a compile time semaphore here?
    if(T8_already_confgured){
        return -1;
    }
    else{
        T8_already_confgured = 1;
        OpenTimer8(T8conval, T8perval);
        ConfigIntTimer8(T8intconval);
        return 0;
    }
}

//T8 ticks getter
unsigned long getT8_ticks(){
    return sys_T8_ticks;
}

//Service enabler
int sysServiceEnableSvcT8(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorT8Enabled[svcNum] = SERVICE_ENABLE;
        return 0;
    }
    else{ return svcNum; }
}
//Service disabler
int sysServiceDisableSvcT8(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorT8Enabled[svcNum] = SERVICE_DISABLE;
        return 0;
    }
    else{ return svcNum; }
}
#endif
///////////////////////////////////////////////////////

////////////////////     Timer 9     //////////////////
#ifdef SYS_SERVICE_T9

unsigned int installedIdxT9 = 0;
unsigned long sys_T9_ticks = 0;
static void (*serviceVectorT9[SERVICE_VECT_LEN])(void);
int serviceVectorT9Enabled[SERVICE_VECT_LEN];
static char T9_already_confgured = 0;
//Timer 9 Service ISR

void __attribute__((interrupt, no_auto_psv)) _T9Interrupt(void) {
    int i;
    for(i=0; i<SERVICE_VECT_LEN; i++){
        if(serviceVectorT9[i] && serviceVectorT9Enabled[i] )
            serviceVectorT9[i]();
    }
    sys_T9_ticks++;
    //Clear Timer9 interrupt flag
    _T9IF = 0;
}

//Timer 9 install function
int sysServiceInstallT9(void* func){
    if(installedIdxT9 < SERVICE_VECT_LEN){
        serviceVectorT9[installedIdxT9] = func;
        serviceVectorT9Enabled[installedIdxT9] = SERVICE_ENABLE;
        installedIdxT9++;
        return (installedIdxT9-1); //succesful install
    }
    return -1; //Error, no more room
}

//Timer 9 setup function
int sysServiceConfigT9(unsigned int T9conval, unsigned int T9perval,
                        unsigned int T9intconval){
    //Todo: is there any way to have a compile time semaphore here?
    if(T9_already_confgured){
        return -1;
    }
    else{
        T9_already_confgured = 1;
        OpenTimer9(T9conval, T9perval);
        ConfigIntTimer9(T9intconval);
        return 0;
    }
}

//T9 ticks getter
unsigned long getT9_ticks(){
    return sys_T9_ticks;
}

//Service enabler
int sysServiceEnableSvcT9(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorT9Enabled[svcNum] = SERVICE_ENABLE;
        return 0;
    }
    else{ return svcNum; }
}
//Service disabler
int sysServiceDisableSvcT9(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorT9Enabled[svcNum] = SERVICE_DISABLE;
        return 0;
    }
    else{ return svcNum; }
}
#endif
///////////////////////////////////////////////////////

