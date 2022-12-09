// RGB PWM Example
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL Evaluation Board
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// Red LED:
//   M1PWM5 (PF1) drives an NPN transistor that powers the red LED
// Green LED:
//   M1PWM7 (PF3) drives an NPN transistor that powers the green LED
// Blue LED:
//   M1PWM6 (PF2) drives an NPN transistor that powers the blue LED

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "clock.h"
#include "wait.h"
#include "tm4c123gh6pm.h"
#include "rgb_led.h"

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize Hardware
void initHw()
{
    // Initialize system clock to 40 MHz
    initSystemClockTo40Mhz();
}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

int main(void)
{
	// Initialize hardware
    initHw();
	initRgb();

    // Cycle through colors
	int16_t i = 0;
	while(true)
	{
		// Off
		setRgbColor(0, 0, 0);
	    waitMicrosecond(1000000);
		// Ramp from off to bright white
	    for (i = 0; i < 1024; i++)
		{
			setRgbColor(i, i, i);
		    waitMicrosecond(10000);
		}
		// Red
		setRgbColor(1023, 0, 0);
	    waitMicrosecond(1000000);
		// Orange
		setRgbColor(1023, 384, 0);
	    waitMicrosecond(1000000);
		// Yellow
		setRgbColor(1023, 1023, 8);
	    waitMicrosecond(1000000);
	    // Green
		setRgbColor(0, 1023, 0);
	    waitMicrosecond(1000000);
		// Cyan
		setRgbColor(0, 1023, 1023);
	    waitMicrosecond(1000000);
		// Blue
		setRgbColor(0, 0, 1023);
	    waitMicrosecond(1000000);
		// Magenta
		setRgbColor(1023, 0, 1023);
	    waitMicrosecond(1000000);
	}
}
