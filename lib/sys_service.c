// sys_service.c

#include "sys_service.h"
#include "timer.h"

#define SERVICE_ENABLE         1
#define SERVICE_DISABLE        0

// Setup Function
void sysServiceSetup(){
//TODO: Decide what goes here
}

////////////////////     Timer 1     //////////////////
#ifdef SYS_SERVICE_T1

unsigned int installedIdxT1 = 0;
unsigned long sys_T1_ticks = 0;
static void (*serviceVectorT1[SERVICE_VECT_LEN])(void);
static unsigned char serviceVectorEnabled[SERVICE_VECT_LEN];
static char T1_already_confgured = 0;
//Timer 1 Service ISR

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    int i;
    for(i=0; i<SERVICE_VECT_LEN; i++){
        if(serviceVectorT1[i] && serviceVectorEnabled[i] )
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
        serviceVectorEnabled[installedIdxT1] = SERVICE_ENABLE;
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
        serviceVectorEnabled[svcNum] = SERVICE_ENABLE;
        return 0;
    }
    else{ return svcNum; }
}
//Service disabler
int sysServiceDisableSvcT1(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorEnabled[svcNum] = SERVICE_DISABLE;
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
static unsigned char serviceVectorEnabled[SERVICE_VECT_LEN];
static char T2_already_confgured = 0;
//Timer 1 Service ISR

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void) {
    int i;
    for(i=0; i<SERVICE_VECT_LEN; i++){
        if(serviceVectorT2[i] && serviceVectorEnabled[i] )
            serviceVectorT2[i]();
    }
    sys_T2_ticks++;
    //Clear Timer1 interrupt flag
    _T2IF = 0;
}

//Timer 1 install function
int sysServiceInstallT2(void* func){
    if(installedIdxT2 < SERVICE_VECT_LEN){
        serviceVectorT2[installedIdxT2] = func;
        serviceVectorEnabled[installedIdxT2] = SERVICE_ENABLE;
        installedIdxT2++;
        return (installedIdxT2-1); //succesful install
    }
    return -1; //Error, no more room
}

//Timer 1 setup function
int sysServiceConfigT2(unsigned int T2conval, unsigned int T2perval,
                        unsigned int T2intconval){
    //Todo: is there any way to have a compile time semaphore here?
    if(T2_already_confgured){
        return -1;
    }
    else{
        T2_already_confgured = 1;
        OpenTimer1(T2conval, T2perval);
        ConfigIntTimer1(T2intconval);
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
        serviceVectorEnabled[svcNum] = SERVICE_ENABLE;
        return 0;
    }
    else{ return svcNum; }
}
//Service disabler
int sysServiceDisableSvcT2(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorEnabled[svcNum] = SERVICE_DISABLE;
        return 0;
    }
    else{ return svcNum; }
}
#endif
///////////////////////////////////////////////////////////


//Timer 3

//Timer 4

////////////////////     Timer 5     //////////////////
#ifdef SYS_SERVICE_T5
unsigned int installedIdxT5 = 0;
unsigned long sys_T5_ticks = 0;
static void (*serviceVectorT5[SERVICE_VECT_LEN])(void);
static unsigned char serviceVectorEnabled[SERVICE_VECT_LEN];
static char T5_already_confgured = 0;
//Timer 1 Service ISR

void __attribute__((interrupt, no_auto_psv)) _T5Interrupt(void) {
    int i;
    for(i=0; i<SERVICE_VECT_LEN; i++){
        if(serviceVectorT5[i] && serviceVectorEnabled[i] )
            serviceVectorT5[i]();
    }
    sys_T5_ticks++;
    //Clear Timer1 interrupt flag
    _T5IF = 0;
}

//Timer 5 install function
int sysServiceInstallT5(void* func){
    if(installedIdxT5 < SERVICE_VECT_LEN){
        serviceVectorT5[installedIdxT5] = func;
        serviceVectorEnabled[installedIdxT5] = SERVICE_ENABLE;
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
        OpenTimer1(T5conval, T5perval);
        ConfigIntTimer1(T5intconval);
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
        serviceVectorEnabled[svcNum] = SERVICE_ENABLE;
        return 0;
    }
    else{ return svcNum; }
}
//Service disabler
int sysServiceDisableSvcT5(unsigned int svcNum){
    if(svcNum < SERVICE_VECT_LEN){
        serviceVectorEnabled[svcNum] = SERVICE_DISABLE;
        return 0;
    }
    else{ return svcNum; }
}
#endif
///////////////////////////////////////////////////////
