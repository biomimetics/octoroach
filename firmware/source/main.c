/******************************************************************************
 * Name: main.c
 * Desc:
 * Date: 2010-07-08
 * Author: stanbaek
 *******************************************************************************/

#include "settings.h"
#include "Generic.h"
#include "p33Fxxxx.h"
#include "init_default.h"
#include "ports.h"
#include "battery.h"
#include "cmd.h"
#include "radio.h"
#include "xl.h"
#include "gyro.h"
#include "utils.h"
#include "sclock.h"
#include "motor_ctrl.h"
#include "led.h"
#include "dfmem.h"
#include "leg_ctrl.h"
#include "pid.h"
#include "adc_pid.h"
#include "steering.h"
#include "telem.h"
#include "hall.h"
#include "tail_ctrl.h"
#include "ams-enc.h"
#include "imu.h"

#include <stdlib.h>

extern unsigned char id[4];

volatile unsigned long wakeTime;
extern volatile char g_radio_duty_cycle;
extern volatile char inMotion;

int dcCounter;

int main(void) {

    wakeTime = 0;
    dcCounter = 0;

    WordVal src_addr_init = {RADIO_SRC_ADDR};
    WordVal src_pan_id_init = {RADIO_SRC_PAN_ID};
    WordVal dst_addr_init = {RADIO_DST_ADDR};

    SetupClock();
    SwitchClocks();
    SetupPorts();
    //batSetup();

    int old_ipl;
    mSET_AND_SAVE_CPU_IP(old_ipl, 1)

    sclockSetup();
    radioInit(src_addr_init, src_pan_id_init, RADIO_RXPQ_MAX_SIZE, RADIO_TXPQ_MAX_SIZE);
    radioSetChannel(RADIO_CHANNEL); //Set to my channel
    macSetDestAddr(dst_addr_init);

    dfmemSetup();
    //xlSetup();
    gyroSetup();
    mcSetup();
    cmdSetup();
    adcSetup();
    telemSetup(); //Timer 5
    encSetup();
    imuSetup();

    #ifdef  HALL_SENSORS
    hallSetup();    // Timer 1, Timer 2
    hallSteeringSetup(); //doesn't exist yet
    #else //No hall sensors, standard BEMF control
    legCtrlSetup(); // Timer 1
    steeringSetup();  //Timer 5
    #endif

    //Tail control is a special case
    //tailCtrlSetup();

    //Camera is untested with current code base, AP 12/6/2012
    //ovcamSetup();

    LED_RED = 1; //Red is use an "alive" indicator
    LED_GREEN = 0;
    LED_YELLOW = 0;

    //Radio startup verification
    if(phyGetState() == 0x16)  { LED_GREEN = 1; }

    //Sleeping and low power options
    //_VREGS = 1;
    //gyroSleep();

    while (1) {
        cmdHandleRadioRxBuffer();

#ifndef __DEBUG //Idle will not work with debug
        //Simple idle:
        if (radioIsRxQueueEmpty()) {
            Idle();
            //_T1IE = 0;
        }
#endif

        //delay_ms(1000);
        //cmdEcho(0, 1 , (unsigned char*)(&i) );
        //i++;
        //if(radioIsRxQueueEmpty() && (t1_ticks >= wakeTime + 5000) ){
        //Idle();
        //LED_RED = 0;
        //gyroSleep();
        //Sleep();
        //}
    }

    /*
    if(g_radio_duty_cycle){
            if(dcCounter == 0){
                    //LED_GREEN = 1;
                    atSetRXAACKON();
            }else{
                    //LED_GREEN = 0;
                    atSetTRXOFF();
            }
    }
    else{
            //LED_GREEN = 1;
    }

    dcCounter = (dcCounter + 1) % 8;
		
    if(radioIsRxQueueEmpty() && !inMotion){
            //gyroSleep();
            LED_RED = 0;
            _SWDTEN = 1; //restart wdt
            Sleep();
            //Idle();
    }
		
    //should be asleep here, waiting for WTD wakeup
    ClrWdt(); //clear wdt
    _SWDTEN = 0; //software disable wdt
    LED_RED = 1;

    //spin up clock
    if(_COSC != 0b010){
            while(OSCCONbits.LOCK!=1);
    }
    //gyroWake();
    }*/
}
