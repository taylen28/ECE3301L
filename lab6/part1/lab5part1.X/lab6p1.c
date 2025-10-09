#include <p18f4620.h>
#include <stdio.h>
#include <math.h>
#include <usart.h>

#pragma config OSC = INTIO67      // Internal oscillator
#pragma config WDT=OFF            // Watchdog Timer disabled
#pragma config LVP=OFF            // Low-Voltage Programming disabled
#pragma config BOREN =OFF         // Brown-Out Reset disabled

#define delay 17000               // Delay value for delay routine

// RGB LED D1 color definitions
#define D1_RED    PORTEbits.RE0
#define D1_GREEN  PORTEbits.RE1
#define D1_BLUE   PORTEbits.RE2

// RGB LED D2 color definitions
#define D2_RED    PORTCbits.RC2
#define D2_GREEN  PORTCbits.RC3
#define D2_BLUE   PORTCbits.RC4

// 7-segment display decimal point
#define SEC_LED PORTDbits.RD7

const char seven_seg_table[10] = {
    0xC0, // 0
    0xF9, // 1
    0xA4, // 2
    0xB0, // 3
    0x99, // 4
    0x92, // 5
    0x82, // 6
    0xF8, // 7
    0x80, // 8
    0x90  // 9
};

void Display_Upper(char num)
{
    PORTD = seven_seg_table[num];   // display number on upper 7-segment
}

void Display_Lower(char num)
{
    PORTC = seven_seg_table[num];   // display number on lower 7-segment
}

void Clear_Upper_Display(void)
{
    PORTD = 0xFF;   // all segments off (common-anode)
}

void Clear_Lower_Display(void)
{
    PORTC = 0xFF;
}


void Wait_One_Second ()
{
    SEC_LED = 1; // First, turn on the SEC LED
    Wait_Half_Second (); // Wait for half second (or 500 msec)
    SEC_LED = 0; // then turn off the SEC LED
    Wait_Half_Second (); // Wait for half second (or 500 msec)
}

void Wait_Half_Second ()
{
    T0CON = 0x03 // Timer 0, 16-bit mode, prescaler 1:16
    TMR0L = 0xEE; // set the lower byte of TMR
    TMR0H = 0x85; // set the upper byte of TMR
    INTCONbits.TMR0IF = 0; // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1; // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0); // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0; // turn off the Timer 0
}

void init_UART ()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
    USART_BRGH_HIGH, 25);
    OSCCON = 0x70;
}

void Wait_N_Seconds (char seconds)
{
char I;
 for (I = 0; I< seconds; I++)
 {
 Wait_One_Second ();
 }
} 

#define NS_RED PORTAbits.RA1
#define NS_GREEN PORTAbits.RA2     
#define EW_RED PORTCbits.RC4
#define EW_GREEN PORTCbits.RC5
#define EWLT_RED PORTBbits.RB7
#define EWLT_GREEN PORTEbits.RE0
#define NSLT_RED PORTCbits.RC2
#define NSLT_GREEN PORTCbits.RC3
#define OFF 0
#define RED 1
#define GREEN 1
#define YELLOW 1
#define UPPER_7SEG LATB
#define LOWER_7SEG LATD
//#define BUZZER PORTDbits.RD1

void Set_NS (char color)
{
 switch (color)
 {
 case OFF: NS_RED =0;NS_GREEN=0;break; // Turns off the NS LED
 case RED: NS_RED =1;NS_GREEN=0;break; // Sets NS LED RED
 case GREEN: NS_RED =0;NS_GREEN=1;break; // sets NS LED GREEN
 case YELLOW: NS_RED =1;NS_GREEN=1;break; // sets NS LED YELLOW
 }
} 

void set_NSLT(char color)
{
    switch(color)
    {
        case OFF: NSLT_RED =0;NSLT_GREEN=0;break; 
        case RED: NSLT_RED =1;NSLT_GREEN=0;break;
        case GREEN: NSLT_RED =0;NSLT_GREEN=1;break; 
        case YELLOW: NSLT_RED =1;NSLT_GREEN=1;break; 
    }
}

void set_EW(char color)
{
    switch(color)
    {
        case OFF: EW_RED =0;EW_GREEN=0;break; 
        case RED: EW_RED =1;EW_GREEN=0;break;
        case GREEN: EW_RED =0;EW_GREEN=1;break; 
        case YELLOW: EW_RED =1;EW_GREEN=1;break; 
    }
}

void set_EWLT(char color)
{
    switch(color)
    {
        case OFF: EWLT_RED =0;EWLT_GREEN=0;break; 
        case RED: EWLT_RED =1;EWLT_GREEN=0;break;
        case GREEN: EWLT_RED =0;EWLT_GREEN=1;break; 
        case YELLOW: EWLT_RED =1;EWLT_GREEN=1;break; 
    }
}



Void PED_Control (char Direction, char Num_Sec) 
{
   if(Direction == 0)
   {
        while(NUM_Sec > 0)
        {
            Display_Upper(NUM_SEC - 1);
            Wait_N_Seconds(1);
            Num_Sec -= 1;
        }
        Clear_Upper_Display();    
   }
   else
   {
        while(NUM_Sec > 0)
        {
            Display_Lower(NUM_Sec - 1);
            Wait_N_Seconds(1);
            Num_Sec--;
        }
        Clear_Lower_Display();
   }
}

void Wait_One_Second_With_Beep ()
{
    SEC_LED = 1; // First, turn on the SEC LED
    Activate_Buzzer () // Activate the buzzer
    Wait_Half_Second (); // Wait for half second (or 500 msec)
    SEC_LED = 0; // then turn off the SEC LED
    Deactivate_Buzzer (); // Deactivate the buzzer
    Wait_Half_Second (); // Wait for half second (or 500 msec)
}
void Activate_Buzzer()
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b01001010 ;
    CCP2CON = 0b00111100 ;
}
void Deactivate_Buzzer()
{
    CCP2CON = 0x0;
    PORTCbits.RC1 = 0;
}

int main()
{
    TRISA = 0xFF;
    TRISB = 0x00;
    TRISC = 0x00;
    TRISD = 0x00;
    TRISE = 0x00;
    
    while(1)
    {
        EW_Green
    }
}