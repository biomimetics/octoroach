/**
\brief ImageProc2.4 specific definition of the "debugpins" bsp module.

\author Andrew Pullin <pullin@berkeley.edu>, January 2013.
 */

// Public functions

void debugpins_init();

// RD2
void debugpins1_toggle();
void debugpins1_clr();
void debugpins1_set();

// RD3
void debugpins2_toggle();
void debugpins2_clr();
void debugpins2_set();

// RD4
void debugpins3_toggle();
void debugpins3_clr();
void debugpins3_set();


// RD5
void debugpins4_toggle();
void debugpins4_clr();
void debugpins4_set();

// RD0
void debugpins5_toggle();
void debugpins5_clr();
void debugpins5_set();

// RD1
void debugpins6_toggle();
void debugpins6_clr();
void debugpins6_set();

void debugpins_wiggle();