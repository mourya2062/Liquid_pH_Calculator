// Stepper Motor Library
// Mourya

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    -

// Hardware configuration:
//Stepper Motor interface through PORT E
//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "clock.h"
#include "tm4c123gh6pm.h"
#include "wait.h"
#include "uart0.h"
#include "Stepper_motor.h"

// Port E Bitband aliases
#define PE2_GREEN       (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 2*4)))
#define PE3_YELLOW      (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 3*4)))
#define PE4_BLACK       (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 4*4)))
#define PE5_WHITE       (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 5*4)))

// PortE masks
#define PE2_GREEN_MASK              4
#define PE3_YELLOW_MASK             8
#define PE4_BLACK_MASK              16
#define PE5_WHITE_MASK              32

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
uint8_t phase               = 0     ;
uint8_t position            = 0     ;
uint8_t i                   = 0     ;
uint8_t Req_tube_pos        = 0     ;
uint8_t Present_tube_pos    = 0     ;
int8_t  Tube_distance       = 0     ;
char     code_str[20]       ={0}    ;
//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

//This function initialize the stepper motor and set the home position
void initStepperMotor()
{
    //Enable Clock for PORT E
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4     ;
    _delay_cycles(3);

    //Configure Port E

    GPIO_PORTE_DIR_R    |=  PE2_GREEN_MASK | PE3_YELLOW_MASK | PE4_BLACK_MASK | PE5_WHITE_MASK       ;   //configured as outputs
    GPIO_PORTE_DR2R_R   |=  PE2_GREEN_MASK | PE3_YELLOW_MASK | PE4_BLACK_MASK | PE5_WHITE_MASK       ;   //setting output current to 2mA
    GPIO_PORTE_DEN_R    |=  PE2_GREEN_MASK | PE3_YELLOW_MASK | PE4_BLACK_MASK | PE5_WHITE_MASK       ;   //Port E data enable

    //calling home function
    home();
}

// Apply phase function which drives the stepper motor
void Apply_Phase(uint8_t phase_value)
{
    switch(phase_value)
    {
    case 0:
        PE4_BLACK           = 1     ;
        PE5_WHITE           = 0     ;
        PE3_YELLOW          = 0     ;
        PE2_GREEN           = 0     ;
        break;
    case 1:
        PE3_YELLOW          = 1     ;
        PE2_GREEN           = 0     ;
        PE4_BLACK           = 0     ;
        PE5_WHITE           = 0     ;
        break;
    case 2:
        PE4_BLACK           = 0     ;
        PE5_WHITE           = 1     ;
        PE3_YELLOW          = 0     ;
        PE2_GREEN           = 0     ;
        break;
    case 3:
        PE2_GREEN           = 1     ;
        PE4_BLACK           = 0     ;
        PE5_WHITE           = 0     ;
        PE3_YELLOW          = 0     ;
        break;
    }
    waitMicrosecond(10000);
}

//stepCw() drives the motor in clock wise direction
void stepCw()
{
      phase = (phase + 1)%4           ;
      Apply_Phase(phase)              ;
      position++                      ;
}

//stepCcw() drives the motor in anti clock wise direction
void stepCcw()
{
      phase = (uint8_t)(phase - 1)%4  ;
      Apply_Phase(phase)              ;
      position--                      ;
}

//setPosition() used to setup the stepper motor position
void setPosition(uint8_t position_value )
{
    position = position_value       ;
}

//home() is used to center the reference tube
void home()
{
    uint8_t find_position = 0       ;
    while(find_position <200)
    {
        stepCw()                    ;
        find_position++             ;
    }

    for(i=0 ; i<5 ;i++)
    {
        stepCcw()   ;
        find_position--  ;
    }

    setPosition(find_position)  ;
}

//goto_tube() changes the position of the tube based on the tube_value() input
void goto_tube(uint8_t tube_value)
{
    switch(tube_value)
       {
       case 0:
           Present_tube_pos   = position    ;
           Req_tube_pos       = 195         ;//197
           if(Req_tube_pos > Present_tube_pos)
           {
               for(i=0;i<(Req_tube_pos-Present_tube_pos);i++)
               {
                   stepCw();
               }
           }
           else
           {
            for (i = 0; i < (Present_tube_pos-Req_tube_pos); i++)
            {
                stepCcw();
            }
           }
           break;

       case 1:
           Present_tube_pos   = position    ;
           Req_tube_pos       = 28;//30         ;
           if(Req_tube_pos > Present_tube_pos)
           {
               for(i=0;i<(Req_tube_pos-Present_tube_pos);i++)
               {
                   stepCw();
               }
           }
           else
           {
            for (i = 0; i < (Present_tube_pos-Req_tube_pos); i++)
            {
                stepCcw();
            }
           }
           break;

       case 2:
           Present_tube_pos   = position    ;
           Req_tube_pos       = 62;//65         ;
           if(Req_tube_pos > Present_tube_pos)
           {
               for(i=0;i<(Req_tube_pos-Present_tube_pos);i++)
               {
                   stepCw();
               }
           }
           else
           {
            for (i = 0; i < (Present_tube_pos-Req_tube_pos); i++)
            {
                stepCcw();
            }
           }
           break;

       case 3:
           Present_tube_pos   = position    ;
           Req_tube_pos       = 96;//98         ;
           if(Req_tube_pos > Present_tube_pos)
           {
               for(i=0;i<(Req_tube_pos-Present_tube_pos);i++)
               {
                   stepCw();
               }
           }
           else
           {
            for (i = 0; i < (Present_tube_pos-Req_tube_pos); i++)
            {
                stepCcw();
            }
           }
           break;

       case 4:
           Present_tube_pos   = position    ;
           Req_tube_pos       = 129;//131         ;
           if(Req_tube_pos > Present_tube_pos)
           {
               for(i=0;i<(Req_tube_pos-Present_tube_pos);i++)
               {
                   stepCw();
               }
           }
           else
           {
            for (i = 0; i < (Present_tube_pos-Req_tube_pos); i++)
            {
                stepCcw();
            }
           }
           break;


       case 5:
           Present_tube_pos   = position    ;
           Req_tube_pos       = 162;//161;//164         ;
           if(Req_tube_pos > Present_tube_pos)
           {
               for(i=0;i<(Req_tube_pos-Present_tube_pos);i++)
               {
                   stepCw();
               }
           }
           else
           {
            for (i = 0; i < (Present_tube_pos-Req_tube_pos); i++)
            {
                stepCcw();
            }
           }
           break;
       }
}


