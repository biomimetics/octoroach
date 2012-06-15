// sys_service.c

#include "sys_service.h"
#include "timer.h"



//This is a generic macro that will allow us to define all the timer
//handlers below in a compact manner.
#define SYS_SERVICE_BUILD(TN) \
        unsigned int installed##TN = 0;\
        unsigned long sys_##TN##_ticks = 0;\
        static void (*serviceVector##TN[SERVICE_VECT_LEN])(void);\
        /* TN ISR function */\
        void __attribute__((interrupt, no_auto_psv)) _##TN##Interrupt(void) {\
            int i;\
            for(i=0; i<SERVICE_VECT_LEN; i++){\
                if(serviceVector##TN[i]) serviceVector##TN[i]();\
            }\
            sys_##TN##_ticks += 1; /* clear TN interrupt flag */\
            _##TN##IF = 0;\
        }\
        int sysTimerInstall##TN (void* func){\
            if(installed##TN < SERVICE_VECT_LEN){\
                installed##TN += 1;\
                serviceVector##TN[installed##TN] = func;\
                return 0; \
            }\
            return -1; \
        }


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
        installedIdxT1++;
        serviceVectorT1[installedIdxT1] = func;
        return 0; //succesful install
    }
    return -1; //Error, no more room
}

//Timer 1 setup function
int sysServiceConfigT1(unsigned int t1conval, unsigned int t1perval,
                        unsigned int t1intconval){
    //Todo: is there any way to have a compile time semaphore here?
    if(T1_already_confgured){
        return -1;
    } 
    else{
        T1_already_confgured = 1;
        OpenTimer1(t1conval, t1perval);
        ConfigIntTimer1(t1intconval);
        return 0;
    }
}

//T1 ticks getter
unsigned long getT1_ticks(){
    return sys_T1_ticks;
}

#endif