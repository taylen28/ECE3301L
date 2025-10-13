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
// === Function prototypes ===
void Wait_Half_Second(void);
void Wait_One_Second(void);
void Wait_N_Seconds(char seconds);
void Wait_One_Second_With_Beep(void);
void Activate_Buzzer(void);
void Deactivate_Buzzer(void);

const char seven_seg_table[10] = {
    0xff, 0x4f, 0x12, 0x06, 0x4c,0x24,0x20,0x0f,0x00,0x04
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

void Wait_Half_Second ()
{
    T0CON = 0x03;
    TMR0L = 0xEE;
    TMR0H = 0x85;
    INTCONbits.TMR0IF = 0;
    T0CONbits.TMR0ON = 1;   
    while (INTCONbits.TMR0IF == 0);
    T0CONbits.TMR0ON = 0;
}


void Wait_One_Second ()
{
    SEC_LED = 1; // First, turn on the SEC LED
    Wait_Half_Second (); // Wait for half second (or 500 msec)
    SEC_LED = 0; // then turn off the SEC LED
    Wait_Half_Second (); // Wait for half second (or 500 msec)
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
#define GREEN 2
#define YELLOW 3
#define UPPER_7SEG PORTB
#define LOWER_7SEG PORTD
#define MODE_LED PORTEbits.RE2
//#define BUZZER PORTDbits.RD1

void set_NS (char color)
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



void PED_Control (char Direction, char Num_Sec) 
{
   if(Direction == 0)
   {
        while(Num_Sec > 0)
        {
            Display_Upper(Num_Sec - 1);
            Wait_One_Second_With_Beep();
            Num_Sec -= 1;
        }
        Clear_Upper_Display();    
   }
   else
   {
        while(Num_Sec > 0)
        {
            Display_Lower(Num_Sec - 1);
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
    Activate_Buzzer (); // Activate the buzzer
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
    // Step 0: Turn off pedestrian displays and mode LED
    UPPER_7SEG = 0;
    LOWER_7SEG = 0;
    MODE_LED = 0;

    // === Step 1: Initial state ===
    set_EW(RED);       // EW RED
    set_EWLT(RED);     // EWLT RED
    set_NSLT(RED);     // NSLT RED
    set_NS(GREEN);     // NS GREEN

    // === Step 2: NS stays GREEN for 6 sec ===
    Wait_N_Seconds(6);

    // === Step 3: NS YELLOW for 3 sec ===
    set_NS(YELLOW);
    Wait_N_Seconds(3);

    // === Step 4: NS RED ===
    set_NS(RED);

    // === Step 5: Check EWLT_SW ===
    if (EWLT_SW == 1)
    {
        // Step 6: EWLT GREEN for 7 sec
        set_EWLT(GREEN);
        Wait_N_Seconds(7);

        // Step 7: EWLT YELLOW for 3 sec
        set_EWLT(YELLOW);
        Wait_N_Seconds(3);

        // Step 8: EWLT RED
        set_EWLT(RED);
    }

    // === Step 9: EW GREEN for 6 sec ===
    set_EW(GREEN);
    Wait_N_Seconds(6);

    // === Step 10: EW YELLOW for 3 sec ===
    set_EW(YELLOW);
    Wait_N_Seconds(3);

    // === Step 11: EW RED ===
    set_EW(RED);

    // === Step 12: Check NSLT_SW ===
    if (NSLT_SW == 1)
    {
        // Step 13: NSLT GREEN for 8 sec
        set_NSLT(GREEN);
        Wait_N_Seconds(8);

        // Step 14: NSLT YELLOW for 3 sec
        set_NSLT(YELLOW);
        Wait_N_Seconds(3);

        // Step 15: NSLT RED
        set_NSLT(RED);
    }

    // === Step 16: Night cycle complete ===
    // Function ends ? main() will call Night_Mode() again if still dark
}

void Day_Mode(void)
{
    // Step 1: Turn on MODE LED and set initial states
    MODE_LED = 1;
    set_EW(RED);
    set_EWLT(RED);
    set_NSLT(RED);
    set_NS(GREEN);

    // Step 2: Check NSPED_SW
    if (NSPED_SW == 1)
    {
        PED_Control(0, 8);       // 0 = NS direction pedestrian countdown
    }

    // Step 3: Wait 7 seconds with NS green
    Wait_N_Seconds(7);

    // Step 4: NS yellow for 3 seconds
    set_NS(YELLOW);
    Wait_N_Seconds(3);

    // Step 5: NS red
    set_NS(RED);

    // Step 6: Check EWLT_SW
    if (EWLT_SW == 1)
    {
        // Step 7: EWLT green for 8 seconds
        set_EWLT(GREEN);
        Wait_N_Seconds(8);

        // Step 8: EWLT yellow for 3 seconds
        set_EWLT(YELLOW);
        Wait_N_Seconds(3);

        // Step 9: EWLT red
        set_EWLT(RED);
    }

    // Step 10: EW green
    set_EW(GREEN);

    // Step 10a: Check EWPED_SW
    if (EWPED_SW == 1)
    {
        PED_Control(1, 8);      // 1 = EW direction pedestrian countdown
    }

    // Step 11: EW stays green for 6 seconds
    Wait_N_Seconds(6);

    // Step 12: EW yellow for 3 seconds
    set_EW(YELLOW);
    Wait_N_Seconds(3);

    // Step 13: EW red
    set_EW(RED);

    // Step 14: Check NSLT_SW
    if (NSLT_SW == 1)
    {
        // Step 15: NSLT green for 7 seconds
        set_NSLT(GREEN);
        Wait_N_Seconds(7);

        // Step 16: NSLT yellow for 3 seconds
        set_NSLT(YELLOW);
        Wait_N_Seconds(3);

        // Step 17: NSLT red
        set_NSLT(RED);
    }

    // Step 18: Done ? Day_Mode sequence complete
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
    TRISA = 0b11100001;   // RA0 input (ADC), RA3-5 inputs (switches), RA1-2 outputs (NS lights)
    TRISB = 0x00;         // RB all outputs
    TRISC = 0x00;         // RC all outputs
    TRISD = 0x00;         // RD all outputs
    TRISE = 0x00;         // RE all outputs
}

int main()
{
   Init_TRIS();
   init_UART();
   Init_ADC();

   while(1)
   {
        unsigned int adc_value = get_full_ADC();    
//        float voltage_mv = adc_value * 4.8828;
        float voltage_v = adc_value * 0.0048828;

        if(voltage_v > 2.50){
            
            Day_Mode();
        }
        else{
            Night_Mode();
        }
        
   }

   
}