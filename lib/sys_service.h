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

#ifdef SYS_SERVICE_T1
int sysServiceInstallT1(void* func);
int sysServiceConfigT1(unsigned int T1conval, unsigned int T1perval,
                        unsigned int T1intconval);
unsigned long getT1_ticks();
#endif

#ifdef SYS_SERVICE_T5
int sysServiceInstallT5(void* func);
int sysServiceConfigT5(unsigned int T5conval, unsigned int t1perval,
                        unsigned int T5intconval);
unsigned long getT5_ticks();
#endif

#endif	//__SYS_SERVICE_H

