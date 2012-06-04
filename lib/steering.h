void steeringSetup(void);
void setSteeringAngRate(int angRate);
void UpdatePIDSteering(pidT *pid, int y);
void steeringSetGains(int Kp,int Ki,int Kd,int Kawm, int ff);
void setSampleSaveCount(int count);
void pidSaveTelemetry(void);

//#define GYRO_AVG_SAMPLES 128
#define GYRO_AVG_SAMPLES 32
#define SATVAL       5000
//#define STEERING_KP  25
//#define STEERING_KI  2
//#define STEERING_KD  0
//#define STEERING_KAW 2
#define STEERING_KP  0
#define STEERING_KI  0
#define STEERING_KD  0
#define STEERING_KAW 0
#define STEERING_GAIN_SCALER 512
