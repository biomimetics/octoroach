#ifndef __PID_H
#define __PID_H

//Select DSP core PID
#define PID_HARDWARE

#ifdef PID_SOFTWARE
#define DEFAULT_KP  200
#define DEFAULT_KI  5
#define DEFAULT_KD  0
#define DEFAULT_KAW 5
#define DEFAULT_FF  0*/

#elif defined PID_HARDWARE
#define DEFAULT_KP  15000
#define DEFAULT_KI  500
#define DEFAULT_KD  150
#define DEFAULT_KAW 0
#define DEFAULT_FF  0
#endif

//Software PID only
#define GAIN_SCALER         100
//#ifndef ADC_MAX
#define ADC_MAX             1024
//#endif

#define NUM_PIDS	2


//Structures and enums
//PID Continer structure
typedef struct
{
	int input;
    long dState, iState, preSat, p, i, d;
    int Kp, Ki, Kd, Kaw, y_old, output;
    unsigned char N;
	char onoff; //boolean
	long error;
	unsigned long run_time;
	unsigned long start_time;
	int inputOffset;
	int feedforward;
	char OUTPUT_CHANNEL;
} pidT;

//Functions
void UpdatePID(pidT *pid, int y);
void pidSetup();
void initPIDObj(pidT *pid, int Kp, int Ki, int Kd, int Kaw, int ff);
void SetupTimer1(void);
void pidSetInput(unsigned int pid_num, int input_val, unsigned int run_time);
void pidSetInputSameRuntime(unsigned int pid_num, int input_val);
void pidSetGains(unsigned int pid_num, int Kp, int Ki, int Kd, int Kaw, int ff);
unsigned char* pidGetTelemetry(void);
void pidOn(unsigned int pid_num);

#endif // __PID_H
