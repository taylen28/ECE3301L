
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>

#pragma config OSC  =   INTIO67   
#pragma config WDT  =   OFF 
#pragma config LVP  =   OFF 
#pragma config BOREN  =   OFF

void main()
{
    /*************************************************************/
    /* Port/Register intialization
    /*************************************************************/
    char in_sw;
        TRISA = 0x00;
        TRISB = 0xFF;
        ADCON1 = 0x0F;
        TRISC = 0x00;
        TRISD = 0x00;
     /*************************************************************/
     /*  loop for defining inputs and outputs
     /*************************************************************/
        while(1)
        {
            in_sw = PORTB;
            in_sw = in_sw & 0x07;
                    
            PORTA = in_sw;
        }
              
}