#include <p18f4525.h>

#pragma config OSC  =   INTIO67   
#pragma config WDT  =   OFF 
#pragma config LVP  =   OFF 
#pragma config BOREN  =   OFF
void main(void)
{
    /*************************************************************/
    /* Port/Register intialization
    /*************************************************************/
    char in_sw;
    TRISB = 0xFF;    // All PORTB pins as input (RB0-RB2 for switches)
    TRISC = 0x00;    // All PORTC pins as output (RC0-RC2 for RGB LED)
    ADCON1 = 0x0F; 
    TRISA = 0x00;
    TRISD = 0x00;
    /*************************************************************/
    /* loop assigning port values output or input
    /*************************************************************/
     while(1)
        {
            in_sw = PORTB;
            in_sw = in_sw & 0x07;
                    
            PORTC = in_sw;
        }
}
