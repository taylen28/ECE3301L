
#include <stdio.h> 
#include <stdlib.h> 
#include <xc.h> 
#include <math.h> 
#include <p18f4620.h> 
 
#pragma config OSC = INTIO67 
#pragma config WDT = OFF 
#pragma config LVP = OFF 
#pragma config BOREN = OFF 

// Prototype Area to place all the references to the routines used in the program 
 /*************************************************************/
 /* initalzation of array of colors for 2 LED's/ outputs
 /*************************************************************/
void Delay_One_Sec();

char array1[8]={0x06, 0x01, 0x02, 0x03, 0x04, 0x00, 0x05, 0x07};
char array2[8]={0x02, 0x03, 0x04, 0x05, 0x00, 0x06, 0x07, 0x01};
void main(void) 
{ 
    char in;
    TRISA =0xFF;        // make PORTA as all inputs
    TRISB =0x00;        // make PORTB as all outputs 
    TRISC =0x00;        // make PORTC as all outputs
    TRISD = 0x00;
    ADCON1 =0x0F;       // make the register digital
     /*************************************************************/
    /* loop traversing each array to change colors
    /*************************************************************/
    while (1)
    {
        for (char i=0; i<8; i++)
        {
            PORTC = array1[i];
            PORTD = array2[i]<<3;
            Delay_One_Sec();
        }
    }
} 

 /*************************************************************/
 /* Delay function defenition
 /*************************************************************/
#define delay 17880
void Delay_One_Sec()
{
 for (int I=0; I <delay; I++);
}

