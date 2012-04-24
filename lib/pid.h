#ifndef __PID_H
#define __PID_H

#define DEFAULT_KP  100
#define DEFAULT_KI  5
#define DEFAULT_KD  0
#define DEFAULT_KAW 5
#define DEFAULT_FF  0

#define GAIN_SCALER         100
#define NUM_PIDS	2

#ifndef ADC_MAX
#define ADC_MAX             1024
#endif

//Structures and enums
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
void pidSetInput(int pid_num, int input_val, unsigned int run_time);
void pidSetInputSameRuntime(int pid_num, int input_val);
void pidSetGains(int pid_num, int Kp, int Ki, int Kd, int Kaw, int ff);
unsigned char* pidGetTelemetry(void);
void pidOn(int pid_num);
void calibBatteryOffset(int spindown_ms);

#endif // __PID_H
