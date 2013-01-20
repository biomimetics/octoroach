// Contents of this file are copyright Andrew Pullin, 2013

/* 
 * File:   sys_service.h
 * Author: pullin
 *
 * Created on June 14, 2012, 3:02 PM
 */

#ifndef __SYS_SERVICE_H
#define	__SYS_SERVICE_H

#define SERVICE_VECT_LEN 8

//This will pull in sysService enabling definitions
#include "settings.h"
#include "timer.h"

////////////////////     Timer 1     //////////////////
#ifdef SYS_SERVICE_T1
int sysServiceInstallT1(void* func);
int sysServiceConfigT1(unsigned int T1conval, unsigned int T1perval,
                        unsigned int T1intconval);
unsigned long getT1_ticks();
int sysServiceEnableSvcT1(unsigned int svcNum);
int sysServiceDisableSvcT1(unsigned int svcNum);
#endif

////////////////////     Timer 2     //////////////////
#ifdef SYS_SERVICE_T2
int sysServiceInstallT2(void* func);
int sysServiceConfigT2(unsigned int T2conval, unsigned int T2perval,
                        unsigned int T2intconval);
unsigned long getT2_ticks();
int sysServiceEnableSvcT2(unsigned int svcNum);
int sysServiceDisableSvcT2(unsigned int svcNum);
#endif

////////////////////     Timer 3     //////////////////
#ifdef SYS_SERVICE_T3
int sysServiceInstallT3(void* func);
int sysServiceConfigT3(unsigned int T3conval, unsigned int T3perval,
                        unsigned int T3intconval);
unsigned long getT3_ticks();
int sysServiceEnableSvcT3(unsigned int svcNum);
int sysServiceDisableSvcT3(unsigned int svcNum);
#endif

////////////////////     Timer 4     //////////////////
#ifdef SYS_SERVICE_T4
int sysServiceInstallT4(void* func);
int sysServiceConfigT4(unsigned int T4conval, unsigned int T4perval,
                        unsigned int T4intconval);
unsigned long getT4_ticks();
int sysServiceEnableSvcT4(unsigned int svcNum);
int sysServiceDisableSvcT4(unsigned int svcNum);
#endif

////////////////////     Timer 5     //////////////////
#ifdef SYS_SERVICE_T5
int sysServiceInstallT5(void* func);
int sysServiceConfigT5(unsigned int T5conval, unsigned int T5perval,
                        unsigned int T5intconval);
unsigned long getT5_ticks();
int sysServiceEnableSvcT5(unsigned int svcNum);
int sysServiceDisableSvcT5(unsigned int svcNum);
#endif

////////////////////     Timer 6     //////////////////
#ifdef SYS_SERVICE_T6
int sysServiceInstallT6(void* func);
int sysServiceConfigT6(unsigned int T6conval, unsigned int T6perval,
                        unsigned int T6intconval);
unsigned long getT6_ticks();
int sysServiceEnableSvcT6(unsigned int svcNum);
int sysServiceDisableSvcT6(unsigned int svcNum);
#endif

////////////////////     Timer 7     //////////////////
#ifdef SYS_SERVICE_T7
int sysServiceInstallT7(void* func);
int sysServiceConfigT7(unsigned int T7conval, unsigned int T7perval,
                        unsigned int T7intconval);
unsigned long getT7_ticks();
int sysServiceEnableSvcT7(unsigned int svcNum);
int sysServiceDisableSvcT7(unsigned int svcNum);
#endif

////////////////////     Timer 8     //////////////////
#ifdef SYS_SERVICE_T8
int sysServiceInstallT8(void* func);
int sysServiceConfigT8(unsigned int T8conval, unsigned int T8perval,
                        unsigned int T8intconval);
unsigned long getT8_ticks();
int sysServiceEnableSvcT8(unsigned int svcNum);
int sysServiceDisableSvcT8(unsigned int svcNum);
#endif

////////////////////     Timer 9     //////////////////
#ifdef SYS_SERVICE_T9
int sysServiceInstallT9(void* func);
int sysServiceConfigT9(unsigned int T9conval, unsigned int T9perval,
                        unsigned int T9intconval);
unsigned long getT9_ticks();
int sysServiceEnableSvcT9(unsigned int svcNum);
int sysServiceDisableSvcT9(unsigned int svcNum);
#endif

#endif	//__SYS_SERVICE_H

