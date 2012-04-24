/*
 * Name: SetupADC.c
 * Desc: Configure the analog to digital converter.
 * Date: 2009-04-02
 * Author: fgb
 */

#include "adc.h"
#include "adc_pid.h"
#include "p33Fxxxx.h"
#include "ports.h"

#define MAX_CHNUM 15 // Highest Analog input number enabled for alternate sampling

int BufferA[MAX_CHNUM+1] __attribute__((space(dma),aligned(2)));
int BufferB[MAX_CHNUM+1] __attribute__((space(dma),aligned(2)));

unsigned int readings[16];

void adcSetup(void){
	adcSetupPeripheral();
}

void adcSetupPeripheral(void){
	unsigned int AD1CON1value, AD1CON2value, AD1CON3value, AD1CON4value, AD1PCFGHvalue, AD1PCFGLvalue, AD1CSSHvalue, AD1CSSLvalue, AD1CHS0value, AD1CHS123value;
    AD1CON1value = ADC_MODULE_ON & ADC_IDLE_CONTINUE & 
                   ADC_AD12B_10BIT & ADC_FORMAT_INTG & ADC_CLK_MPWM &
                   ADC_MULTIPLE & ADC_AUTO_SAMPLING_ON & ADC_SAMP_ON;
    AD1CON2value = ADC_VREF_AVDD_AVSS & ADC_SCAN_OFF & ADC_SELECT_CHAN_0 &
                   ADC_DMA_ADD_INC_2 & ADC_ALT_BUF_OFF & ADC_ALT_INPUT_OFF;
    AD1CON3value = ADC_CONV_CLK_SYSTEM & ADC_CONV_CLK_3Tcy & ADC_SAMPLE_TIME_1;
    AD1CON4value = ADC_DMA_BUF_LOC_1;
    AD1PCFGHvalue = ENABLE_ALL_DIG_16_31;
    AD1PCFGLvalue = ENABLE_AN1_ANA & ENABLE_AN11_ANA;    
    AD1CSSHvalue = SCAN_NONE_16_31;
    AD1CSSLvalue = 0x000F; // Enabling AN0-3
    AD1CHS0value = ADC_CH0_NEG_SAMPLEA_VREFN & ADC_CH0_POS_SAMPLEA_AN11;
	AD1CHS123value = 0x0000;//ADC_CH123_NEG_SAMPLEA_VREFN & ADC_CH123_POS_SAMPLEA_3_4_5;

    SetChanADC1(AD1CHS123value, AD1CHS0value);
    OpenADC1(AD1CON1value, AD1CON2value, AD1CON3value, AD1CON4value, AD1PCFGLvalue, AD1PCFGHvalue, AD1CSSHvalue, AD1CSSLvalue);

	AD1CHS0value = ADC_CH0_NEG_SAMPLEA_VREFN & ADC_CH0_POS_SAMPLEA_AN1;
	SetChanADC2(AD1CHS123value, AD1CHS0value);
    OpenADC2(AD1CON1value, AD1CON2value, AD1CON3value, AD1CON4value, AD1PCFGLvalue, AD1PCFGHvalue, AD1CSSHvalue, AD1CSSLvalue);
	IFS0bits.AD1IF = 0; // Clear the A/D interrupt flag bit
	IEC0bits.AD1IE = 0; //Enable A/D interrupt
}


//For testing purposes
void __attribute__((interrupt,no_auto_psv)) _ADC1Interrupt(void)
{
	//ADC sync indicator
	//if(AD1CON1bits.DONE){
	if(AD1CON1bits.DONE){
		LATB |= (1<<4);
	}else{
		LATB &= ~(1<<4);
	}
	//IFS0bits.AD1IF = 0;
}
