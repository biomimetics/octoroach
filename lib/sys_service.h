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
#include "pullin_const.h"
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
#ifdef SYS_SERVICE_T5
int sysServiceInstallT5(void* func);
int sysServiceConfigT5(unsigned int T5conval, unsigned int T5perval,
                        unsigned int T5intconval);
unsigned long getT5_ticks();
int sysServiceEnableSvcT5(unsigned int svcNum);
int sysServiceDisableSvcT5(unsigned int svcNum);
#endif

#endif	//__SYS_SERVICE_H

