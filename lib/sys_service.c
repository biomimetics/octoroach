// sys_service.c

#include "sys_service.h"
#include "timer.h"


// Setup Function
void sysServiceSetup(){
//TODO: Decide what goes here
}

///// Timer 1 /////
#ifdef SYS_SERVICE_T1

unsigned int installedIdxT1 = 0;
unsigned long sys_T1_ticks = 0;
static void (*serviceVectorT1[SERVICE_VECT_LEN])(void);
static char T1_already_confgured = 0;
//Timer 1 Service ISR

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    int i;
    for(i=0; i<SERVICE_VECT_LEN; i++){
        if(serviceVectorT1[i]) serviceVectorT1[i]();
    }
    sys_T1_ticks++;
    //Clear Timer1 interrupt flag
    _T1IF = 0;
}

//Timer 1 install function
int sysServiceInstallT1(void* func){
    if(installedIdxT1 < SERVICE_VECT_LEN){
        serviceVectorT1[installedIdxT1] = func;
        installedIdxT1++;
        return 0; //succesful install
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
#endif

//Timer 2

//Timer 3

//Timer 4

///// Timer 5 /////
#ifdef SYS_SERVICE_T5

unsigned int installedIdxT5 = 0;
unsigned long sys_T5_ticks = 0;
static void (*serviceVectorT5[SERVICE_VECT_LEN])(void);
static char T5_already_confgured = 0;
//Timer 1 Service ISR

void __attribute__((interrupt, no_auto_psv)) _T5Interrupt(void) {
    int i;
    for(i=0; i<SERVICE_VECT_LEN; i++){
        if(serviceVectorT5[i]) serviceVectorT5[i]();
    }
    sys_T5_ticks++;
    //Clear Timer1 interrupt flag
    _T5IF = 0;
}

//Timer 1 install function
int sysServiceInstallT5(void* func){
    if(installedIdxT5 < SERVICE_VECT_LEN){
        serviceVectorT5[installedIdxT5] = func;
        installedIdxT5++;
        return 0; //succesful install
    }
    return -1; //Error, no more room
}

//Timer 1 setup function
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
#endif