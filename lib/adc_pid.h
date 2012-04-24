void adcSetupPeripheral(void);
void initDMA0(void);
void adcSetup(void);
unsigned int* getADCReadings();

#ifndef ADC_MAX
#define ADC_MAX             853
#endif
