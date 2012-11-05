#ifndef __PID_H
#define __PID_H

// better to turn gains to zero until initialized by command
#define DEFAULT_KP  0
#define DEFAULT_KI  0
#define DEFAULT_KD  0
#define DEFAULT_KAW 0
#define DEFAULT_FF  0

#define GAIN_SCALER         100
#define NUM_PIDS	2
#define NUM_VELS	4 // 8 velocity setpoints per cycle
// actual gear ratio 21.3:1. So with 2 counts/rev, get 42.6:1
#define COUNT_REVS  32   // depends on gear ratio- counts per leg rev
// STRIDE_TICKS should be easily divisible
#define STRIDE_TICKS (COUNT_REVS*16)  // number of t1 ticks/leg revolution
/* The back emf constant can be measured by measuring velocity from Hall Effect sensor */
#define K_EMF    ((1000/256)* 0.27) //458.8/522.6)  // A/D units per hall count per ms, scale by >>8

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
} pidT;

// pid type for leg control
typedef struct
{
	long p_input;	// reference position input - mult by 16
	int v_input; // reference velocity input
  	long preSat; // output value before saturations
	long  p, i, d;   // control contributions from position, integral, and derivative gains respectively
      int Kp, Ki, Kd;
	int Kaw;  // anti-windup gain
	int  output;	 //  control output u
 	char onoff; //boolean
	long p_error;  // position error
	int v_error; // velocity error
	long i_error; // integral error
	unsigned long run_time;
	unsigned long start_time;
	int inputOffset;
	int feedforward;
    int phase_offset;
} pidPos;

// telemetry control structure 
typedef struct
{	char onoff;				// telemetry recording enabled 
	unsigned long start;	// recording start time   
	int count;				// count of samples to record
	int skip;				// samples to skip
} TelemStruct;

// structure for velocity control of leg cycle
typedef struct
{ 
	int interpolate;  				// intermediate value between setpoints
	unsigned long expire;		// end of current segment
	int index;					// right index to moves
	int interval[NUM_VELS];	// number of ticks between intervals
	int delta[NUM_VELS];   // increments for right setpoint
	int vel[NUM_VELS];     // velocity increments to setpoint, >>8
	int leg_stride;
} pidVelLUT;


//Functions
void UpdatePID(pidPos *pid);
void pidSetup();
void initPIDVelProfile();
void setPIDVelProfile(int pid_num, int *interval, int *delta, int *vel);
void initPIDObj(pidT *pid, int Kp, int Ki, int Kd, int Kaw, int ff);
void initPIDObjPos(pidPos *pid, int Kp, int Ki, int Kd, int Kaw, int ff);
void SetupTimer1(void);
void pidSetInput(int pid_num, int input_val, unsigned int run_time);
void pidSetInputSameRuntime(int pid_num, int input_val);
void pidSetGains(int pid_num, int Kp, int Ki, int Kd, int Kaw, int ff);
void pidGetState(int *measurements);
void pidGetSetpoint();
void pidSetControl();
void EmergencyStop(void);
unsigned char* pidGetTelemetry(void);
void pidOn(int pid_num);
void pidZeroPos(int pid_num);
void calibBatteryOffset(int spindown_ms);

#endif // __PID_H
