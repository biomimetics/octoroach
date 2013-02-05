typedef struct {
    unsigned long timeStamp;
    int inputL;
    int inputR;
    int dcL;
    int dcR;
    int gyroX;
    int gyroY;
    int gyroZ;
    int gyroAvg;
    int accelX;
    int accelY;
    int accelZ;
    int bemfL;
    int bemfR;
    float tailTorque;
    int Vbatt;
    int steerAngle;
    float tailAngle;
    float bodyPosition;
    unsigned long motor_count[2];
    int sOut;
} orTelemStruct_t;


void orTelemGetData(char* ptr);

unsigned int orTelemGetSize();