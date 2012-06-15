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

int test __attribute__ ((weak));

void sysServiceSetup(){
    int b = test;
}

#define SYS_SERVICE_T1


#ifdef SYS_SERVICE_T1
SYS_SERVICE_BUILD(T1)
/*
 * unsigned int installedT1 = 0;
unsigned long sys_T1_ticks = 0;
static void (*serviceVectorT1[SERVICE_VECT_LEN])(void);

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
static int sysTimerInstallT1(void* func){
    if(installedT1 < SERVICE_VECT_LEN){
        installedT1++;
        serviceVectorT1[installedT1] = func;
        return 0; //succesful install
    }
    return -1; //Error, no more room
}*/
#endif

#ifdef SYS_SERVICE_T3
SYS_SERVICE_BUILD(T3)
#endif

#ifdef SYS_SERVICE_T4
SYS_SERVICE_BUILD(T4)
#endif

#ifdef SYS_SERVICE_T5
SYS_SERVICE_BUILD(T5)
#endif

#ifdef SYS_SERVICE_T6
SYS_SERVICE_BUILD(T6)
#endif