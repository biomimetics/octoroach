/**
\brief ImageProc2.4 specific definition of the "debugpins" bsp module.

\author Andrew Pullin <pullin@berkeley.edu>, January 2013.
 */

#include "p33fj128mc706a.h"
#include "debugpins.h"

void debugpins_init() {
    //Debug pins on camera connector
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;
    TRISDbits.TRISD3 = 0;
    TRISDbits.TRISD4 = 0;
    TRISDbits.TRISD5 = 0;
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

// RD3
void debugpins2_toggle() {
    _RD3 ^= 0x1;
}

void debugpins2_clr() {
    _RD3 = 0x0;
}

void debugpins2_set() {
    _RD3 = 0x1;
}

// RD4
void debugpins3_toggle() {
    _RD4 ^= 0x1;
}

void debugpins3_clr() {
    _RD4 = 0x0;
}

void debugpins3_set() {
    _RD4 = 0x1;
}

// RD5
void debugpins4_toggle() {
    _RD5 ^= 0x1;
}

void debugpins4_clr() {
    _RD5 = 0x0;
}

void debugpins4_set() {
    _RD5 = 0x1;
}

// RD0
void debugpins5_toggle() {
    _RD0 ^= 0x1;
}

void debugpins5_clr() {
    _RD0 = 0x0;
}

void debugpins5_set() {
    _RD0 = 0x1;
}

// RD1
void debugpins6_toggle() {
    _RD1 ^= 0x1;
}

void debugpins6_clr() {
    _RD1 = 0x0;
}

void debugpins6_set() {
    _RD1 = 0x1;
}
