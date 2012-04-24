/***************************************************************************
* Name: cmd.c
* Desc: Receiving and transmitting queue handler
* Date: 2010-07-10
* Author: stanbaek
**************************************************************************/

#include "cmd.h"
#include "cmd_const.h"
#include "dfmem.h"
#include "utils.h"
#include "ports.h"
#include "gyro.h"
#include "xl.h"
#include "stopwatch.h"
#include "led.h"
#include "motor_ctrl.h"
#include "payload.h"
#include "dfmem.h"
#include "pid.h"
#include "radio.h"
#include "move_queue.h"
#include "steering.h"
#include "dfmem_extra.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

unsigned char tx_frame_[127];

extern MoveQueue moveq;
extern int offsz;

extern moveCmdT currentMove, idleMove;

// use an array of function pointer to avoid a number of case statements
// CMD_VECTOR_SIZE is defined in cmd_const.h
void (*cmd_func[CMD_VECTOR_SIZE])(unsigned char, unsigned char, unsigned char*);

/*-----------------------------------------------------------------------------
 *          Declaration of static functions
-----------------------------------------------------------------------------*/
static void cmdSetThrust(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSteer(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdGetImuData(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdGetImuLoop(unsigned char status, unsigned char length, unsigned char *frame);

static void cmdStartImuDataSave(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdStopImuDataSave(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdTxSavedImuData(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdEraseMemSector(unsigned char status, unsigned char length, unsigned char *frame);

//static void cmdEcho(unsigned char status, unsigned char length, unsigned char *frame);
void cmdEcho(unsigned char status, unsigned char length, unsigned char *frame);

static void cmdNop(unsigned char status, unsigned char length, unsigned char *frame);

//User commands
static void cmdSetThrustOpenLoop(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSetThrustClosedLoop(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSetPIDGains(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdGetPIDTelemetry(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSetCtrldTurnRate(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdGetImuLoopZGyro(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSetMoveQueue(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSetSteeringGains(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSoftwareReset(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdSpecialTelemetry(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdEraseSector(unsigned char status, unsigned char length, unsigned char *frame);
static void cmdFlashReadback(unsigned char status, unsigned char length, unsigned char *frame);

/*-----------------------------------------------------------------------------
 *          Public functions
-----------------------------------------------------------------------------*/
void cmdSetup(void) {

    unsigned int i;

    // initialize the array of func pointers with Nop()
    for(i = 0; i < MAX_CMD_FUNC; ++i) {
        cmd_func[i] = &cmdNop;
    }

    cmd_func[CMD_ECHO] = &cmdEcho;
    cmd_func[CMD_SET_THRUST] = &cmdSetThrust;
    cmd_func[CMD_SET_STEER] = &cmdSteer;    
    cmd_func[CMD_GET_IMU_DATA] = &cmdGetImuData;
    cmd_func[CMD_GET_IMU_LOOP] = &cmdGetImuLoop;
    cmd_func[CMD_START_IMU_SAVE] = &cmdStartImuDataSave;
    cmd_func[CMD_STOP_IMU_SAVE] = &cmdStopImuDataSave;
    cmd_func[CMD_TX_SAVED_IMU_DATA] = &cmdTxSavedImuData;
    cmd_func[CMD_ERASE_MEM_SECTOR] = &cmdEraseMemSector;
	//Use commands
	cmd_func[CMD_SET_THRUST_OPENLOOP] = &cmdSetThrustOpenLoop;
	cmd_func[CMD_SET_THRUST_CLOSEDLOOP] = &cmdSetThrustClosedLoop;
	cmd_func[CMD_SET_PID_GAINS] = &cmdSetPIDGains;
	cmd_func[CMD_GET_PID_TELEMETRY] = &cmdGetPIDTelemetry;
	cmd_func[CMD_SET_CTRLD_TURN_RATE] = &cmdSetCtrldTurnRate;
	cmd_func[CMD_GET_IMU_LOOP_ZGYRO] = &cmdGetImuLoopZGyro;
	cmd_func[CMD_SET_MOVE_QUEUE] = &cmdSetMoveQueue;
	cmd_func[CMD_SET_STEERING_GAINS] = &cmdSetSteeringGains;
	cmd_func[CMD_SOFTWARE_RESET] = &cmdSoftwareReset;
	cmd_func[CMD_SPECIAL_TELEMETRY] = &cmdSpecialTelemetry;
	cmd_func[CMD_ERASE_SECTORS] = &cmdEraseSector;
	cmd_func[CMD_FLASH_READBACK] = &cmdFlashReadback;
}


void cmdHandleRadioRxBuffer(void) {

    Payload pld;
    unsigned char command, status;  

    if ((pld = radioReceivePayload()) != NULL) {

        status = payGetStatus(pld);
       command = payGetType(pld);      
        
        cmd_func[command](status, pld->data_length, payGetData(pld));

        payDelete(pld);
    } 

    return;
}



/*-----------------------------------------------------------------------------
 * ----------------------------------------------------------------------------
 * The functions below are intended for internal use, i.e., private methods.
 * Users are recommended to use functions defined above.
 * ----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

static void cmdSetThrust(unsigned char status, unsigned char length, unsigned char *frame) {

    unsigned char chr_test[4];
    float *duty_cycle = (float*)chr_test;

    chr_test[0] = frame[0];
    chr_test[1] = frame[1];
    chr_test[2] = frame[2];
    chr_test[3] = frame[3];
    
    mcSetDutyCycle(MC_CHANNEL_PWM1, duty_cycle[0]);
	//mcSetDutyCycle(1, duty_cycle[0]);
}


static void cmdSteer(unsigned char status, unsigned char length, unsigned char *frame){
    
    unsigned char chr_test[4];
    float *steer_value = (float*)chr_test;

    chr_test[0] = frame[0];
    chr_test[1] = frame[1];
    chr_test[2] = frame[2];
    chr_test[3] = frame[3];

    mcSteer(steer_value[0]);
}


/*-----------------------------------------------------------------------------
 *          IMU functions
-----------------------------------------------------------------------------*/
static void cmdGetImuData(unsigned char status, unsigned char length, unsigned char *frame) {

	//This function is deprecated
	//It relies on sensors.h , which is not compatible with this project currently.
	
	//senGetIMUData(status, CMD_GET_IMU_DATA);
    
	/*unsigned char *xl_data, *gyro_data;
    unsigned char i;
    Payload pld = payCreateEmpty(14);  // data length = 12

    xlReadXYZ();
    xl_data = xlGetsXYZ();
    gyroReadXYZ();
    gyro_data = gyroGetsXYZ();
    for(i = 0; i < 6; ++i) {
        pld->pld_data[i] = xl_data[i];
        pld->pld_data[i+6] = gyro_data[i];
    }

    pld->status = status;
    pld->type = CMD_GET_IMU_DATA;
    radioTxPayload(pld);
    */
}


// return packet format:
// 4 bytes for time
// 6 bytes for xl data
// 6 bytes for gyro data
static void cmdGetImuLoop(unsigned char status, unsigned char length, unsigned char *frame) {

    unsigned int count;
    unsigned long tic;
    unsigned char *tic_char;
    Payload pld;
  
    LED_RED = 1;

    count = frame[0] + (frame[1] << 8);

    tic_char = (unsigned char*)&tic;
    swatchReset();
    tic = swatchTic();
    
    while (count) {

        pld = payCreateEmpty(16);  // data length = 16
        paySetData(pld, 4, tic_char);
        payAppendData(pld, 4, 6, xlReadXYZ());
        payAppendData(pld,10, 6, gyroReadXYZ());
        paySetStatus(pld, status);
        paySetType(pld, CMD_GET_IMU_DATA);

        radioSendPayload(macGetDestAddr(),pld);
        count--;
		payDelete(pld);
        delay_ms(4);  
        tic = swatchTic();
    }

    LED_RED = 0;

}


static void cmdStartImuDataSave(unsigned char status, unsigned char length, unsigned char *frame) {
    //Deprecated due to dependance on sensors.h
	//Use cmdSpecialTelemetry().
	//senSetImuDataSave(1);  
}

static void cmdStopImuDataSave(unsigned char status, unsigned char length, unsigned char *frame) {
	//Deprecated due to dependance on sensors.h
	//Use cmdSpecialTelemetry().
    //senSetImuDataSave(0);  
}

static void cmdTxSavedImuData(unsigned char status, unsigned char length, unsigned char *frame) {
    
    unsigned int page, byte;
    unsigned int i, j;
    Payload pld;

    //senGetMemLocIndex(&page, &byte);
    page = 0x0200;
    byte = 0;

    LED_RED = 1;

    dfmemEraseSector(0x0100);   // erase Sector 1 (page 256 - 511)

    for (i = 0x0100; i < 0x0200; ++i) {
        j = 0;
        while (j < 512) {
            pld = payCreateEmpty(18);  // data length = 16
            dfmemRead(i, j, 16, pld->pld_data);
            paySetStatus(pld, status);
            paySetType(pld, CMD_GET_IMU_DATA);
            while(!radioReceivePayload());
            j += 16;
        }
        delay_ms(200);
    }

    LED_RED = 0;
}



static void cmdEraseMemSector(unsigned char status, unsigned char length, unsigned char *frame) {
    unsigned int page;
	page = frame[0] + (frame[1] << 8);
    LED_RED = 1;
    dfmemEraseSector(0x0100);   // erase Sector 1 (page 256 - 511)
    LED_RED = 0;
}





/*-----------------------------------------------------------------------------
 *          AUX functions
-----------------------------------------------------------------------------*/
void cmdEcho(unsigned char status, unsigned char length, unsigned char *frame) {
     
    radioSendPayload(macGetDestAddr(), payCreate(length, frame, status, CMD_ECHO));
}

static void cmdNop(unsigned char status, unsigned char length, unsigned char *frame) {
    Nop();
}


/*-----------------------------------------------------------------------------
 *         User function
-----------------------------------------------------------------------------*/
static void cmdSetThrustOpenLoop(unsigned char status, unsigned char length, unsigned char *frame){
	//unsigned char chan1[4], chan2[4];
    //float *duty_cycle1 = (float*)chan1;
	//float *duty_cycle2 = (float*)chan2;
	int dc1, dc2;

	dc1 = *(int*)(frame);
	dc2 = *(int*)(frame + sizeof(int) );

    /*chan2[0] = frame[0];
    chan2[1] = frame[1];
    chan2[2] = frame[2];
    chan2[3] = frame[3];

	chan1[4] = frame[4];
    chan1[5] = frame[5];
    chan1[6] = frame[6];
    chan1[7] = frame[7];*/
    
	PDC1 = dc1;
	PDC2 = dc2;

    //mcSetDutyCycle(MC_CHANNEL_PWM1, duty_cycle1[0]);
	//mcSetDutyCycle(MC_CHANNEL_PWM2, duty_cycle2[0]);
	//mcSetDutyCycle(1, duty_cycle[0]);
}

static void cmdSetThrustClosedLoop(unsigned char status, unsigned char length, unsigned char *frame){
	int i;
	int chan1 = frame[0] + (frame[1] << 8);
	unsigned int run_time_ms1 = frame[2] + (frame[3] << 8);
	int chan2 = frame[4] + (frame[5] << 8);
	unsigned int run_time_ms2 = frame[6] + (frame[7] << 8);
	//currentMove = manualMove;
	pidSetInput(0 ,chan1, run_time_ms1);
	pidOn(0);
	pidSetInput(1 ,chan2, run_time_ms2);
	pidOn(1);
	//delay_ms(2);

	i=0;
	if((chan1 > 0) || (chan2 > 0)){
		i++;
		i++;
	}

	unsigned int telem_samples = frame[8] + (frame[9] << 8);

	
	
	//unsigned char temp[2];
	//*(unsigned int*)temp = 1000;
	if(telem_samples > 0){
		cmdGetPIDTelemetry(0, 2, (unsigned char*)(&telem_samples));
	}
}

static void cmdSetPIDGains(unsigned char status, unsigned char length, unsigned char *frame){
	int Kp, Ki, Kd, Kaw, ff;
	int idx = 0;
	Payload pld;
	Kp = frame[idx] + (frame[idx+1] << 8); idx+=2;
	Ki = frame[idx] + (frame[idx+1] << 8); idx+=2;
	Kd = frame[idx] + (frame[idx+1] << 8); idx+=2;
	Kaw = frame[idx] + (frame[idx+1] << 8); idx+=2;
	ff = frame[idx] + (frame[idx+1] << 8); idx+=2;
	pidSetGains(0,Kp,Ki,Kd,Kaw, ff);
	Kp = frame[idx] + (frame[idx+1] << 8); idx+=2;
	Ki = frame[idx] + (frame[idx+1] << 8); idx+=2;
	Kd = frame[idx] + (frame[idx+1] << 8); idx+=2;
	Kaw = frame[idx] + (frame[idx+1] << 8); idx+=2;
	ff = frame[idx] + (frame[idx+1] << 8); idx+=2;
	pidSetGains(1,Kp,Ki,Kd,Kaw, ff);
	//Send confirmation packet
	pld = payCreateEmpty(20);
	pld->pld_data[0] = status;
    pld->pld_data[1] = CMD_SET_PID_GAINS;
	memcpy((pld->pld_data)+2, frame, 20);
	radioSendPayload((WordVal)macGetDestAddr(), pld);
}

static void cmdGetPIDTelemetry(unsigned char status, unsigned char length, unsigned char *frame){
	unsigned int count;
	unsigned long tic;
    unsigned char *tic_char = (unsigned char*)&tic;
	//unsigned long sampNum = 0;
	int i;
	unsigned short idx = 0;
	Payload pld;
	unsigned char* telem_ptr;

	count = frame[0] + (frame[1] << 8);
	swatchReset();
    tic = swatchTic();
	
	while(count){
		pld = payCreateEmpty(36);  // data length = 12
	
		//*(long*)(pld->pld_data + idx) = tic;
		pld->pld_data[2] = tic_char[0];
        pld->pld_data[3] = tic_char[1];
        pld->pld_data[4] = tic_char[2];
        pld->pld_data[5] = tic_char[3];
		idx += sizeof(tic);

		telem_ptr = pidGetTelemetry();

        
		//memcpy((pld->pld_data)+idx , telem_ptr, 4*sizeof(int));
		for(i = 0; i < (4*sizeof(int)+6*sizeof(long)); ++i) {
            pld->pld_data[i+6] = telem_ptr[i];
        }

        pld->pld_data[0] = status;
        pld->pld_data[1] = CMD_GET_PID_TELEMETRY;
        radioSendPayload(macGetDestAddr(), pld);
        count--;
        //delay_ms(2);   // ~3ms delay
        //delay_us(695);
		delay_ms(10);
        tic = swatchTic();
	}
}

static void cmdSetCtrldTurnRate(unsigned char status, unsigned char length, unsigned char *frame){
	int rate;
	Payload pld;
	rate = frame[0] + (frame[1] << 8);
	setSteeringAngRate(rate);
	
	//Send confirmation packet
	pld = payCreateEmpty(sizeof(int));
	pld->pld_data[0] = status;
    pld->pld_data[1] = CMD_SET_CTRLD_TURN_RATE;
	memcpy((pld->pld_data)+2, frame, sizeof(int));
	radioSendPayload((WordVal)macGetDestAddr(), pld);	
}


static void cmdGetImuLoopZGyro(unsigned char status, unsigned char length, unsigned char *frame) {

    unsigned int count;
    unsigned long tic;
    unsigned char *tic_char;
    Payload pld;
    //int perpacket = 10;
	int perpacket = 2;
    int i;
    int packidx = 0;
    int dc1, dc2;
    //LED_RED = 1;
	int intwz;
	unsigned char gdata[6];
	unsigned char* gdatap = gdata;
	int* zp = (int*)(gdatap + 4);
	

    count = frame[0] + (frame[1] << 8);

    tic_char = (unsigned char*)&tic;
    	
    swatchReset();

    while (count) {
        pld = payCreateEmpty(10*perpacket); 
        packidx = 0;
        for(i = 0; i < perpacket; i++){

            tic = swatchTic();
            //if (tic == 4) //testing why tic seems to reset at ~1.2 seconds
            //{
            //        asm volatile("nop");
            //        asm volatile("nop");
            //}
			gyroGetXYZ(gdata);
			intwz = *zp - offsz;

            payAppendData(pld, packidx, 4, tic_char); //time , long , 4 bytes
            packidx += 4;
            payAppendData(pld, packidx, 2, (unsigned char*)(&intwz) );  //gyro data , int, 2 bytes
            packidx += 2;
            dc1 = PDC1; dc2 = PDC2;
            payAppendData(pld, packidx, 2, (unsigned char*)(&dc1) );  //duty cycle 1, int, 2 bytes
            packidx += 2;
            payAppendData(pld, packidx, 2, (unsigned char*)(&dc2) );  //duty cycle 2 , int, 2 bytes
            packidx += 2;
            count--;
            delay_ms(4);
        }
		
        paySetStatus(pld, status);
        paySetType(pld, CMD_GET_IMU_LOOP_ZGYRO);

        radioSendPayload(macGetDestAddr(),pld);
      
    }

    //LED_RED = 0;

}

static void cmdSetMoveQueue(unsigned char status, unsigned char length, unsigned char *frame) {
	unsigned int count;
	int idx = 0;
	count = (unsigned int)(*(frame+idx));
	idx += 2;
	moveCmdT move;
	int i;
	unsigned char durtemp[4];
	unsigned long* dur = (unsigned long*)durtemp; // requires for unsigned long read
	
	for(i = 0; i < count; i++){
		move = (moveCmdT)malloc(sizeof(moveCmdStruct));
		//move->inputL = (int)(*(frame+idx));
		move->inputL = frame[idx] + (frame[idx+1] << 8); idx+=2;
		//idx += 2;
		//move->inputR = (int)(*(frame+idx));
		move->inputR = frame[idx] + (frame[idx+1] << 8); idx+=2;
		//idx += 2;
		//apparently this is required to properly read the unsigned long.
		durtemp[0] = frame[idx];
		durtemp[1] = frame[idx+1];
		durtemp[2] = frame[idx+2];
		durtemp[3] = frame[idx+3];
		move->duration = dur[0];
		idx += 4;
		mqPush(moveq, move);
	}
}

static void cmdSetSteeringGains(unsigned char status, unsigned char length, unsigned char *frame){
	int Kp, Ki, Kd, Kaw, ff;
	int idx = 0;
	Payload pld;
	Kp = frame[idx] + (frame[idx+1] << 8); idx+=2;
	Ki = frame[idx] + (frame[idx+1] << 8); idx+=2;
	Kd = frame[idx] + (frame[idx+1] << 8); idx+=2;
	Kaw = frame[idx] + (frame[idx+1] << 8); idx+=2;
	ff = frame[idx] + (frame[idx+1] << 8); idx+=2;
	steeringSetGains(Kp,Ki,Kd,Kaw, ff);
	//Send confirmation packet
	pld = payCreateEmpty(10);
	pld->pld_data[0] = status;
    pld->pld_data[1] = CMD_SET_STEERING_GAINS;
	memcpy((pld->pld_data)+2, frame, 10);
	radioSendPayload((WordVal)macGetDestAddr(), pld);
}

static void cmdSoftwareReset(unsigned char status, unsigned char length, unsigned char *frame)
{
	asm volatile("reset");
}

static void cmdSpecialTelemetry(unsigned char status, unsigned char length, unsigned char *frame){
	unsigned int count;
	count = frame[0] + (frame[1] << 8);
	if(count != 0){
		swatchReset();
		setSampleSaveCount(count);
	} 
	//This functionality has been removed to accomodate large numbers of samples
	//else{ //start a readback over the radio
		//shut down all movement before sending
	//	pidSetInput(0,0,0);
	//	pidSetInput(1,0,0);
		//readDFMem();
		//readDFMemBySample(count);
	//}
}

static void cmdEraseSector(unsigned char status, unsigned char length, unsigned char *frame)
{
	unsigned int numSamples = frame[0] + (frame[1] << 8);
	//eraseDFMemSectors0a0b();
	eraseDFMemSectorsForSamples(numSamples);
}

static void cmdFlashReadback(unsigned char status, unsigned char length, unsigned char *frame){
	unsigned int count = frame[0] + (frame[1] << 8);
	readDFMemBySample(count);
}
