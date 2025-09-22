#include <stdio.h> 
#include <stdlib.h> 
#include <xc.h> 
#include <math.h> 
#include <p18f4620.h> 
 
#pragma config OSC = INTIO67 
#pragma config WDT = OFF 
#pragma config LVP = OFF 
#pragma config BOREN = OFF 


 /*************************************************************/
    /*Prototype function call  and port initialization
    /*************************************************************/
void Delay_One_Sec();
void main(void) 
{ 
    TRISA =0xFF;        // make PORTA as all inputs
    TRISB =0x00;        // make PORTB as all outputs 
    TRISC =0x00; 
    TRISD = 0x00;
    // make PORTC as all outputs
    ADCON1 =0x0F;       // make the register digital
    /*************************************************************/
    /* Loop for making port c change colors with Delay function called
    /*************************************************************/
    while (1)
    {
        for (char i=0; i<8; i++)
        {
            PORTC = i;
            Delay_One_Sec();
        }
    }
} 


 /*************************************************************/
 /* Delay_one_sec() function defenition
 /*************************************************************/
#define delay 17000
void Delay_One_Sec()
{
 for (int I=0; I <delay; I++);
}