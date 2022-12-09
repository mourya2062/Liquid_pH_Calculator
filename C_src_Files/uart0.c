// UART0 Library
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    -

// Hardware configuration:
// UART Interface:
//   U0TX (PA1) and U0RX (PA0) are connected to the 2nd controller
//   The USB on the 2nd controller enumerates to an ICDI interface and a virtual COM port

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "string.h"
#include "tm4c123gh6pm.h"
#include "uart0.h"

// PortA masks
#define UART_TX_MASK 2
#define UART_RX_MASK 1

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize UART0
void initUart0()
{
    // Enable clocks
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
    _delay_cycles(3);

    // Configure UART0 pins
    GPIO_PORTA_DR2R_R |= UART_TX_MASK;                  // set drive strength to 2mA (not needed since default configuration -- for clarity)
    GPIO_PORTA_DEN_R |= UART_TX_MASK | UART_RX_MASK;    // enable digital on UART0 pins
    GPIO_PORTA_AFSEL_R |= UART_TX_MASK | UART_RX_MASK;  // use peripheral to drive PA0, PA1
    GPIO_PORTA_PCTL_R &= ~(GPIO_PCTL_PA1_M | GPIO_PCTL_PA0_M); // clear bits 0-7
    GPIO_PORTA_PCTL_R |= GPIO_PCTL_PA1_U0TX | GPIO_PCTL_PA0_U0RX;
                                                        // select UART0 to drive pins PA0 and PA1: default, added for clarity

    // Configure UART0 to 115200 baud, 8N1 format
    UART0_CTL_R = 0;                                    // turn-off UART0 to allow safe programming
    UART0_CC_R = UART_CC_CS_SYSCLK;                     // use system clock (40 MHz)
    UART0_IBRD_R = 21;                                  // r = 40 MHz / (Nx115.2kHz), set floor(r)=21, where N=16
    UART0_FBRD_R = 45;                                  // round(fract(r)*64)=45
    UART0_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN;    // configure for 8N1 w/ 16-level FIFO
    UART0_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN;
                                                        // enable TX, RX, and module
}

// Set baud rate as function of instruction cycle frequency
void setUart0BaudRate(uint32_t baudRate, uint32_t fcyc)
{
    uint32_t divisorTimes128 = (fcyc * 8) / baudRate;   // calculate divisor (r) in units of 1/128,
                                                        // where r = fcyc / 16 * baudRate
    UART0_CTL_R = 0;                                    // turn-off UART0 to allow safe programming
    UART0_IBRD_R = divisorTimes128 >> 7;                // set integer value to floor(r)
    UART0_FBRD_R = ((divisorTimes128 + 1) >> 1) & 63;   // set fractional value to round(fract(r)*64)
    UART0_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN;    // configure for 8N1 w/ 16-level FIFO
    UART0_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN;
                                                        // turn-on UART0
}

// Blocking function that writes a serial character when the UART buffer is not full
void putcUart0(char c)
{
    while (UART0_FR_R & UART_FR_TXFF);               // wait if uart0 tx fifo full
    UART0_DR_R = c;                                  // write character to fifo
}

// Blocking function that writes a string when the UART buffer is not full
void putsUart0(char* str)
{
    uint32_t i = 0;
    while (str[i] != '\0')
        putcUart0(str[i++]);
}

// Blocking function that returns with serial data once the buffer is not empty
char getcUart0()
{
    while (UART0_FR_R & UART_FR_RXFE);               // wait if uart0 rx fifo empty
    return UART0_DR_R & 0xFF;                        // get character from fifo
}

void getsUart0(USER_DATA *struct_data)
{
    uint32_t i = 0;
    while (1)
    {
        //Getting Character from UART 0
        struct_data->buffer[i] = getcUart0();

        //checking whether the character is back space or not if back space do not consider that
        if((struct_data-> buffer[i] == 8)||(struct_data-> buffer[i] == 127))
        {
             i=i-1;
        }

        //Adding null character if character is carraige return
        else if(struct_data-> buffer[i] == 13)
        {
            struct_data-> buffer[i] = '\0';
            return;
        }

        //considering all the characters whose ASCII value is above 32
        else if((struct_data-> buffer[i] >= 32) && (struct_data-> buffer[i] != 127) ){

            i= i+1;

        }

        //if maximum characters are received then add the NULL statement
        if (i == MAX_CHARS)
        {
            struct_data->buffer[i + 1] = '\0';
            return;
        }
    }

}

//This function checks Transition from Delimiter to alpha or numeric
void parseFields(USER_DATA *struct_data)
{
    uint8_t char_count = 0, fieldcount = 0, field_position_type_count = 0;
    char data1, data2;

    //Run the loop till you hit the NULL character
    while ((struct_data->buffer[char_count]) != '\0')
    {

        data1 = (struct_data->buffer[char_count])           ;   //Present value

        if (char_count > 0)
            data2 = (struct_data->buffer[char_count - 1])   ;  // Previous value

        //First Character Transition is assumed from delimiter
        if (char_count == 0)
        {
            //Checking first character is a number or not
            if ((data1 > 47 && data1 < 58))
            {
                struct_data->fieldPosition[field_position_type_count] = char_count;
                struct_data->fieldType[field_position_type_count] = 'n';
                char_count = char_count + 1;
                fieldcount = fieldcount + 1;
                field_position_type_count = field_position_type_count + 1;

            }
            //Checking first character is an alphabet or not
            else if ((data1 > 64 && data1 < 91) || (data1 > 96 && data1 < 123))
            {
                struct_data->fieldPosition[field_position_type_count] = char_count;
                struct_data->fieldType[field_position_type_count] = 'a';
                char_count = char_count + 1;
                fieldcount = fieldcount + 1;
                field_position_type_count = field_position_type_count + 1;
            }
            //If it is not a number or an alphabet then it is a delimiter so just increment the char count we shall replace this with "\0"
            else
                char_count = char_count + 1;

        }
        else
        {
            //Checking whether the old character is a delimiter or not and replacing it with NULL
            if (!((data2 > 47 && data2 < 58) || (data2 > 64 && data2 < 91) || (data2 > 96 && data2 < 123)))
            {
                struct_data->buffer[char_count - 1] = '\0';

                //Recording the Transition type
                if ((data1 > 47 && data1 < 58))
                {
                    struct_data->fieldPosition[field_position_type_count] = char_count;
                    struct_data->fieldType[field_position_type_count] = 'n';
                    char_count = char_count + 1;
                    fieldcount = fieldcount + 1;
                    field_position_type_count = field_position_type_count + 1;

                }
                else if ((data1 > 64 && data1 < 91)
                        || (data1 > 96 && data1 < 123))
                {
                    struct_data->fieldPosition[field_position_type_count] = char_count;
                    struct_data->fieldType[field_position_type_count] = 'a';
                    char_count = char_count + 1;
                    fieldcount = fieldcount + 1;
                    field_position_type_count = field_position_type_count + 1;
                }
                else
                {
                    //Come back here to check whether making NULL is required or not
                    //struct_data->buffer[char_count] = '\0';
                    char_count = char_count + 1;
                }
            }
            else
                char_count = char_count + 1;

        }

    }

    if (fieldcount >= MAX_FIELDS)
        struct_data->fieldCount = MAX_FIELDS;
    else
        struct_data->fieldCount = fieldcount;

}

//return the string base address based on field number
char* getFieldString(USER_DATA* data, uint8_t fieldNumber) {
    if(fieldNumber <= data->fieldCount)
        return &(data->buffer[data->fieldPosition[fieldNumber]])  ;
    else
        return '\0' ;
}

//returns the integer based on field number
int32_t getFieldInteger(USER_DATA* data, uint8_t fieldNumber)   {
    return atoi(&(data->buffer[data->fieldPosition[fieldNumber]]))  ;
}

//isCommand() is used to validate the command
bool isCommand(USER_DATA* data, const char strCommand[],uint8_t minArguments){
    if(minArguments <= data->fieldCount){
        if(strcmp(getFieldString(data,0),strCommand) == 0){
            return true     ;
        }
        else{
            return false    ;
        }
    }
    else{
        return false    ;
    }
}
// Returns the status of the receive buffer
bool kbhitUart0()
{
    return !(UART0_FR_R & UART_FR_RXFE);
}


