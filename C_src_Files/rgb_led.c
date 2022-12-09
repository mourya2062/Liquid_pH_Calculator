// RGB LED Library
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL with LCD Interface
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// Red Backlight LED:
//   M1PWM5 (PF1) drives an NPN transistor that powers the red LED
// Green Backlight LED:
//   M1PWM7 (PF3) drives an NPN transistor that powers the green LED
// Blue Backlight LED:
//   M1PWM6 (PF2) drives an NPN transistor that powers the blue LED

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <rgb_led.h>
#include <stdint.h>
#include "tm4c123gh6pm.h"

// RGB  masks
#define RED_LED_MASK 16     //Port B(4)
#define GREEN_LED_MASK 64   //Port A(6)
#define BLUE_LED_MASK 128   //Port A(7)

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize RGB
void initRgb()
{
    // Enable clocks
    SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R0 | SYSCTL_RCGCPWM_R1;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0 | SYSCTL_RCGCGPIO_R1;
    _delay_cycles(3);


    // Configure BLUE and GREEN LED
    GPIO_PORTA_DEN_R    |=  GREEN_LED_MASK | BLUE_LED_MASK                  ;
    GPIO_PORTA_AFSEL_R  |=  GREEN_LED_MASK | BLUE_LED_MASK                  ;
    GPIO_PORTA_PCTL_R   &=  ~(GPIO_PCTL_PA6_M | GPIO_PCTL_PA7_M)            ;
    GPIO_PORTA_PCTL_R   |=  GPIO_PCTL_PA6_M1PWM2 | GPIO_PCTL_PA7_M1PWM3     ;

    // Configure RED LED port
    GPIO_PORTB_DEN_R    |= RED_LED_MASK                                     ;
    GPIO_PORTB_AFSEL_R  |= RED_LED_MASK                                     ;
    GPIO_PORTB_PCTL_R   &= ~(GPIO_PCTL_PB4_M)                               ;
    GPIO_PORTB_PCTL_R   |= GPIO_PCTL_PB4_M0PWM2                             ;


    // Configure PWM module 1 to drive RGB LED
    // RED   on M0PWM2 (PB4), M0PWM1a
    // BLUE  on M1PWM2 (PA6), M1PWM1a
    // GREEN on M1PWM3 (PA7), M1PWM1b
    SYSCTL_SRPWM_R  = SYSCTL_SRPWM_R1|SYSCTL_SRPWM_R0                           ; // reset PWM1 module
    SYSCTL_SRPWM_R  = 0                                                         ; // leave reset state
    PWM0_1_CTL_R    = 0                                                         ; // turn-off PWM0 generator 1 (drives outs 2 and 3)
    PWM1_1_CTL_R    = 0                                                         ; // turn-off PWM1 generator 1 (drives outs 10 and 11)

    PWM0_1_GENA_R   = PWM_0_GENA_ACTCMPAD_ONE | PWM_0_GENA_ACTLOAD_ZERO         ; // output 2 on PWM0, gen 1a, cmpa
    PWM1_1_GENA_R = PWM_1_GENA_ACTCMPAD_ONE | PWM_1_GENA_ACTLOAD_ZERO           ; // output 4 on PWM1, gen 1a, cmpa
    PWM1_1_GENB_R = PWM_1_GENB_ACTCMPBD_ONE | PWM_1_GENB_ACTLOAD_ZERO           ; // output 5 on PWM1, gen 1b, cmpb

    PWM0_1_LOAD_R = 2048;                            // set frequency to 40 MHz sys clock / 2 / 1024 = 19.53125 kHz
    PWM1_1_LOAD_R = 2048;

    PWM0_1_CMPA_R = 0;                               // red off (0=always low, 1023=always high)
    PWM1_1_CMPB_R = 0;                               // green off
    PWM1_1_CMPA_R = 0;                               // blue off

    PWM0_1_CTL_R    =   PWM_0_CTL_ENABLE                    ; // turn-on PWM0 generator 1 (drives outs 2 )
    PWM1_1_CTL_R    =   PWM_0_CTL_ENABLE                    ; // turn-on PWM1 generator 1 (drives outs 11)

    PWM0_ENABLE_R = PWM_ENABLE_PWM2EN                       ; // enable outputs
    PWM1_ENABLE_R = PWM_ENABLE_PWM2EN | PWM_ENABLE_PWM3EN   ; // enable outputs
}

void setRgbColor(uint16_t red, uint16_t green, uint16_t blue)
{
    PWM0_1_CMPA_R = red;
    PWM1_1_CMPA_R = green;
    PWM1_1_CMPB_R = blue;
}


