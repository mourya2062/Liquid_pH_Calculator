// Stepper Motor Library
//Mourya

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#ifndef STEPPERMOTOR_H_
#define STEPPERMOTOR_H_

//#define DEBUG

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void initStepperMotor()                             ;
void Apply_Phase(uint8_t phase_value)               ;
void stepCw()                                       ;
void stepCcw()                                      ;
void setPosition(uint8_t position_value )           ;
void home()                                         ;
void goto_tube(uint8_t tube_value)                  ;
#endif
