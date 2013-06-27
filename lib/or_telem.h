// Contents of this file are copyright Andrew Pullin, 2013

//or_telem.h , OctoRoACH specific telemetry packet format header

// Data structure type
typedef struct {
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
    int steerIn;
    int steerOut;
    int Vbatt;
    unsigned long motor_count[2];
    float yawAngle;
} orTelemStruct_t;

void orTelemGetData(unsigned char* ptr);

unsigned int orTelemGetSize();