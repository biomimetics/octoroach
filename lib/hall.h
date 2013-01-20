// Modifications and additions to this file made by Andrew Pullin are copyright, 2013
// Copyrights are acknowledged for portions of this code extant before modifications by Andrew Pullin 
// Any application of BSD or other license to copyright content without the authors express approval
// is invalid and void.

#ifndef __HALL_H
#define __HALL_H

// better to turn gains to zero until initialized by command
#define DEFAULT_HALL_KP  0
#define DEFAULT_HALL_KI  0
#define DEFAULT_HALL_KD  0
#define DEFAULT_HALL_KAW 0
#define DEFAULT_HALL_FF  0

#define GAIN_SCALER         100
#define NUM_PIDS	2
#define NUM_VELS	4 // 8 velocity setpoints per cycle
// actual gear ratio 21.3:1. So with 2 counts/rev, get 42.6:1
#define COUNT_REVS  42   // depends on gear ratio- counts per leg rev
// STRIDE_TICKS should be easily divisible
#define STRIDE_TICKS (COUNT_REVS*16)  // number of t1 ticks/leg revolution


#define NUM_HALL_PIDS 2

//Limits on output PWM
#define HALFTHROT 2000
#define FULLTHROT 2*HALFTHROT
//#define MAXTHROT 3976
#define SATTHROT (int)((3976.0/4000.0)*(float)FULLTHROT)


//Structures and enums

// pid type for leg control

typedef struct {
    long p_input; // reference position input - mult by 16
    int v_input; // reference velocity input
    long dState, iState, p, i, d;
    int Kp, Ki, Kd, Kaw, y_old;
    unsigned char N;
    char onoff; //boolean
    long p_error; // position error
    int v_error; // velocity error
    long i_error; // integral error
    unsigned long run_time;
    unsigned long start_time;
    int inputOffset;
    int Kff;
    long preSat; // output value before saturations
    int output; //  control output
    int maxVal, minVal;
    int satValPos, satValNeg;
} pidPos;

// telemetry control structure

//typedef struct {
//    char onoff; // telemetry recording enabled
//    unsigned long start; // recording start time
//    int count; // count of samples to record
//    int skip; // samples to skip
//} TelemStruct;

// structure for velocity control of leg cycle

typedef struct {
    int interpolate; // intermediate value between setpoints
    unsigned long expire; // end of current segment
    int index; // right index to moves
    int interval[NUM_VELS]; // number of ticks between intervals
    int delta[NUM_VELS]; // increments for right setpoint
    int vel[NUM_VELS]; // velocity increments to setpoint, >>8
    int leg_stride;
} hallVelLUT;

//Public Functions
void hallSetup();
void hallInitPIDVelProfile();
void hallSetVelProfile(int pid_num, int *interval, int *delta, int *vel);
void hallInitPIDObj(pidObj *pid, int Kp, int Ki, int Kd, int Kaw, int ff);
void hallInitPIDObjPos(pidPos *pid, int Kp, int Ki, int Kd, int Kaw, int ff);
void hallPIDSetInput(int pid_num, int input_val, unsigned int run_time);
void hallSetInputSameRuntime(int pid_num, int input_val);
void hallSetGains(int pid_num, int Kp, int Ki, int Kd, int Kaw, int ff);
void hallGetState(int *measurements);
void hallPIDOn(int pid_num);
void hallZeroPos(int pid_num);
long* hallGetMotorCounts();

#endif // __HALL_H
