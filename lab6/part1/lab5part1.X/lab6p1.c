#include <p18f4620.h>
#include <stdio.h>
#include <math.h>
#include <usart.h>

#pragma config OSC = INTIO67      // Internal oscillator
#pragma config WDT=OFF            // Watchdog Timer disabled
#pragma config LVP=OFF            // Low-Voltage Programming disabled
#pragma config BOREN =OFF         // Brown-Out Reset disabled

#define delay 17000               // Delay value for delay routine


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
    T0CON = 0x03; // Timer 0, 16-bit mode, prescaler 1:16
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

//switches
#define NSPED_SW PORTAbits.RA3
#define EWPED_SW PORTAbits.RA4
#define NSLT_SW PORTAbits.RA5
#define EWLT_SW PORTCbits.RC0

//outputs
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
#define MODE_LED PORTEbits.RE2
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
        case RED: NSLT_RED =1;NSLT_GREEN =0;break;
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
            Wait_One_Second_With_Beep();
            Num_Sec -= 1;
        }
        Clear_Upper_Display();    
   }
   else
   {
        while(NUM_Sec > 0)
        {
            Display_Lower(NUM_Sec - 1);
            Wait_One_Second_With_Beep();
            Num_Sec--;
        }
        Clear_Lower_Display();
   }
}

//Buzzer implementaion
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

void Night_Mode()
{
    UPPER_7SEG=0;
    LOWER_7SEG=0;
    MODE_LED = 0;
    while(1)
    {
        set_EW(RED);
        set_EWLD(RED);
        set_NSLT(RED);
        set_NS(GREEN);
        Wait_N_Seconds(1);

        set_EW(OFF);
        set_EWLD(OFF);
        set_NSLT(OFF);
        set_NS(OFF);
        Wait_N_Seconds(1);
        
        WAIT_N_Seconds(6);

        set_NS(YELLOW);
        WAIT__N_SECONDS(3);

        set_NS(RED);
        if(EWLT_SW == 1)
        {
            set_EWLT(GREEN);
            WAIT_N_SECONDS(7);
            set_EWLT(YELLOW);
            WAIT_N_SECONDS(3);
            set_EWLT(RED);

            set_EW(GREEN);
            WAIT_N_SECONDS(6);
            set_EW(YELLOW);
            WAIT__N_SECONDS(3);
            set_EW(RED);   

        }
        else
        {
            set_EW(GREEN);
            WAIT_N_SECONDS(6);
            set_EW(YELLOW);
            WAIT__N_SECONDS(3);
            set_EW(RED);   

        }
        if(NSLT_SW == 1)
        {
            set_NSLT(GREEN);
            WAIT__N_SECONDS(8);
            set_NSLT(YELLOW);
            WAIT_N_SECONDS(3);
            set_NSLT(RED);
        }
        else
        {
             set_NSLT(RED);
        }
    }

    void Day_Mode()
    {
        MODE_LED = 1;
        set_EW(RED);
        set_EWLT(RED);
        set_NSLT(RED);
        set_NS(GREEN);

        if(NSPED_SW == 1)
        {
            PED_Control(NS_GREEN, 8);
            WAIT__N_SECONDS(7);
            set_NS(YELLOW);
            WAIT__N_SECONDS(3);
            set_NS(RED);
        }
        else
        {
            WAIT__N_SECONDS(7);
            set_NS(YELLOW);
            WAIT__N_SECONDS(3);
            set_NS(RED);
        }
        if(EWLT_SW == 1)
        {
            set_EWLT(GREEN);
            WAIT__N_SECONDS(8);
            set_EWLT(YELLOW);
            WAIT__N_SECONDS(3);
            set_EWLT(RED);
        }
        else
        {
            set_EW(GREEN);
            if(EWPED_SW == 1)
            {
                PED_Control(EW_GREEN, 8);
                set_EW(GREEN);
                WAIT__N_SECONDS(6);
                set_EW(YELLOW);
                WAIT__N_SECONDS(3);
                set_EW(RED);
            }
            else{
                set_EW(GREEN);
                WAIT__N_SECONDS(6);
                set_EW(YELLOW);
                WAIT__N_SECONDS(3);
                set_EW(RED);
            }
        }
    }
    if(NSLT_SW == 1)
    {
        set_NSLT(GREEN);
        WAIT__N_SECONDS(7);
        set_NSLT(YELLOW);
        WAIT__N_SECONDS(3);
        set_NSLT(RED);

    }else{break;}

}

//ADC functions
void Init_ADC(void) {
    ADCON0 = 0x01;
    ADCON1 = 0x0E;
    ADCON2 = 0xA9;
}
unsigned int get_full_ADC(void) {
    ADCON0bits.GO = 1;
    while (ADCON0bits.DONE == 1); // wait until done
    return (ADRESH << 8) + ADRESL;
}
void Init_TRIS(void) {
    TRISA = 0xFF;   // RA0 input (photo sensor)
    TRISB = 0x00;   // 7-seg lower digit output
    TRISD = 0x00;   // 7-seg upper digit output
}


int main()
{
   Init_TRIS();
   init_UART();
   Init_ADC();

   while(1)
   {
        unsigned int adc_value = get_full_ADC();    
        float voltage_mv = adc_value * 4.8828;
        float voltage_v = adc_value * 0.0048828;

        if(voltage_v > 2.50){
            
            Day_Mode();
        }
        else{
            Night_Mode();
        }
         

   }

   
}