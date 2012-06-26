#ifndef __ADC_PID_H
#define __ADC_PID_H

void adcSetup(void); //Top level config function, to be called from main

//Getters for other modules to access values
unsigned int adcGetBEMFL();
unsigned int adcGetBEMFR();
unsigned int adcGetVBatt();
unsigned int adcGetAN3();

#ifndef ADC_MAX
#define ADC_MAX             853
#endif

#endif //__ADC_PID_H
