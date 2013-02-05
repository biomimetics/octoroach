/**
\brief ImageProc2.4 specific definition of the "debugpins" bsp module.

\author Andrew Pullin <pullin@berkeley.edu>, January 2013.
 */

#include "p33fj128mc706a.h"
#include "debugpins.h"

//Pins are ordinally numbers from outboard (1) to inboard (6)

void debugpins_init() {
    //Debug pins on camera connector
    TRISFbits.TRISF1 = 0;
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;
    TRISDbits.TRISD3 = 0;
    TRISDbits.TRISD4 = 0;
    debugpins1_clr();
    debugpins2_clr();
    debugpins3_clr();
    debugpins4_clr();
    debugpins5_clr();
    debugpins6_clr();
}

// RD2
void debugpins1_toggle() {
    _RD2 ^= 0x1;
}

void debugpins1_clr() {
    _RD2 = 0x0;
}

void debugpins1_set() {
    _RD2 = 0x1;
}

// RD1
void debugpins2_toggle() {
    _RD1 ^= 0x1;
}

void debugpins2_clr() {
    _RD1 = 0x0;
}

void debugpins2_set() {
    _RD1 = 0x1;
}

// RD3
void debugpins3_toggle() {
    _RD3 ^= 0x1;
}

void debugpins3_clr() {
    _RD3 = 0x0;
}

void debugpins3_set() {
    _RD3 = 0x1;
}

// RD0
void debugpins4_toggle() {
    _RD0 ^= 0x1;
}

void debugpins4_clr() {
    _RD0 = 0x0;
}

void debugpins4_set() {
    _RD0 = 0x1;
}

// RD4
void debugpins5_toggle() {
    _RD4 ^= 0x1;
}

void debugpins5_clr() {
    _RD4 = 0x0;
}

void debugpins5_set() {
    _RD4 = 0x1;
}

// RF1
void debugpins6_toggle() {
    _RF1 ^= 0x1;
}

void debugpins6_clr() {
    _RF1 = 0x0;
}

void debugpins6_set() {
    _RF1 = 0x1;
}

//Set then clear all pins. Useful as a trigger, and as a fixture test.
void debugpins_wiggle(){
        //Wiggle all lines as triggers
    debugpins1_set();
    debugpins2_set();
    debugpins3_set();
    debugpins4_set();
    debugpins5_set();
    debugpins6_set();
    delay_ms(50);
    debugpins1_clr();
    debugpins2_clr();
    debugpins3_clr();
    debugpins4_clr();
    debugpins5_clr();
    debugpins6_clr();
    delay_ms(50);
}