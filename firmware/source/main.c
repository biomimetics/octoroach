/******************************************************************************
* Name: main.c
* Desc: 
* Date: 2010-07-08
* Author: stanbaek
*******************************************************************************/

#include "or_const.h"
#include "p33Fxxxx.h"
#include "init_default.h"
#include "ports.h"
#include "battery.h"
#include "cmd.h"
#include "radio.h"
#include "xl.h"
#include "gyro.h"
#include "utils.h"
#include "stopwatch.h"
#include "motor_ctrl.h"
#include "led.h"
#include "dfmem.h"
#include "pid.h"
#include "adc_pid.h"
#include "steering.h"

#include <stdio.h>
#include "stdlib.h"

extern unsigned char id[4];

//char framebuf[80][120] __attribute__((far));;

int main(void) {

    WordVal src_addr_init = {SRC_ADDR};
    WordVal src_pan_id_init = {SRC_PAN_ID};
    WordVal dst_addr_init = {DST_ADDR};

    SetupClock();
    SwitchClocks();
    SetupPorts();
    batSetup();

    swatchSetup();
    radioInit(src_addr_init, src_pan_id_init, RXPQ_MAX_SIZE, TXPQ_MAX_SIZE);
	radioSetChannel(MY_CHAN); //Set to my channel
    macSetDestAddr(dst_addr_init);

    dfmemSetup();
	unsigned char memsize;
	memsize = dfmemGetChipSize();
    xlSetup();
    gyroSetup();
    mcSetup();
    cmdSetup();
    //senSetup();
	adcSetup();
    pidSetup();
    steeringSetup();
	
    //radioReadTrxId(id);

    LED_RED = 1;
    LED_BLUE = 0;
	LED_YELLOW = 0;

	//while(1);

    if(phyGetState() == 0x16)  { LED_GREEN = 1; }

    //print("Ready");	

	//readDFMemBySample(5);

    while(1) {
     	cmdHandleRadioRxBuffer();

		//Simple idle ; reduces idle current to 70 mA
        // TODO (abuchan, apullin, fgb) : Idle() causes unexpected behavior
		//if(radioIsRxQueueEmpty()){
		//	Idle();
		//}

    }
}

