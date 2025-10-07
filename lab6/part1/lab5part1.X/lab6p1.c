#include <p18f4620.h>
#include <usart.h>

// ====================== CONFIG BITS ======================
#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF

// ====================== CLOCK SETUP ======================
#define _XTAL_FREQ 8000000

// ====================== I/O DEFINITIONS ======================
// SEC LED
#define SEC_LED      LATDbits.LATD7

// RGB LEDs (adjust pins according to your schematic)
#define NS_RED       LATAbits.LATA1
#define NS_GREEN     LATAbits.LATA2

#define EW_RED       LATBbits.LATB0
#define EW_GREEN     LATBbits.LATB1

#define NSLT_RED     LATBbits.LATB2
#define NSLT_GREEN   LATBbits.LATB3

#define EWLT_RED     LATCbits.LATC0
#define EWLT_GREEN   LATCbits.LATC2

// MODE LED
#define MODE_LED     LATDbits.LATD0

// INPUT SWITCHES
#define EWLT_SW      PORTAbits.RA3
#define NSLT_SW      PORTAbits.RA4
#define EWPED_SW     PORTAbits.RA5
#define NSPED_SW     PORTAbits.RA6

// PHOTO SENSOR (MODE)
#define MODE_SENSOR  PORTAbits.RA0

// COLORS
#define OFF 0
#define RED 1
#define GREEN 2
#define YELLOW 3

// ====================== FUNCTION PROTOTYPES ======================
void Wait_Half_Second(void);
void Wait_One_Second(void);
void Wait_N_Seconds(char seconds);

void Set_NS(char color);
void Set_EW(char color);
void Set_NSLT(char color);
void Set_EWLT(char color);

void Activate_Buzzer(void);
void Deactivate_Buzzer(void);

void Wait_One_Second_With_Beep(void);
void PED_Control(char Direction, char Num_Sec);

void Day_Mode(void);
void Night_Mode(void);

// ====================== DELAY FUNCTIONS ======================
void Wait_Half_Second(void)
{
    // Timer0: 16-bit, prescaler 1:16
    T0CON = 0x03;        // 16-bit, prescaler 1:16
    // We need 0.5 sec = 500ms; Fosc=8MHz => Ftimer=Fosc/4=2MHz => T=0.5us
    // Counts needed: 500ms/0.5us=1,000,000. /16 prescaler => 62500.
    // 65536-62500=3036 => 0x0BDC
    TMR0H = 0x0B;        
    TMR0L = 0xDC;        
    INTCONbits.TMR0IF = 0;
    T0CONbits.TMR0ON = 1;
    while(INTCONbits.TMR0IF == 0);
    T0CONbits.TMR0ON = 0;
}

void Wait_One_Second(void)
{
    SEC_LED = 1;
    Wait_Half_Second();
    SEC_LED = 0;
    Wait_Half_Second();
}

void Wait_N_Seconds(char seconds)
{
    char i;
    for(i=0; i<seconds; i++)
        Wait_One_Second();
}

// ====================== RGB LED SET FUNCTIONS ======================
void Set_NS(char color){
    switch(color){
        case OFF:    NS_RED=0; NS_GREEN=0; break;
        case RED:    NS_RED=1; NS_GREEN=0; break;
        case GREEN:  NS_RED=0; NS_GREEN=1; break;
        case YELLOW: NS_RED=1; NS_GREEN=1; break;
    }
}

void Set_EW(char color){
    switch(color){
        case OFF:    EW_RED=0; EW_GREEN=0; break;
        case RED:    EW_RED=1; EW_GREEN=0; break;
        case GREEN:  EW_RED=0; EW_GREEN=1; break;
        case YELLOW: EW_RED=1; EW_GREEN=1; break;
    }
}

void Set_NSLT(char color){
    switch(color){
        case OFF:    NSLT_RED=0; NSLT_GREEN=0; break;
        case RED:    NSLT_RED=1; NSLT_GREEN=0; break;
        case GREEN:  NSLT_RED=0; NSLT_GREEN=1; break;
        case YELLOW: NSLT_RED=1; NSLT_GREEN=1; break;
    }
}

void Set_EWLT(char color){
    switch(color){
        case OFF:    EWLT_RED=0; EWLT_GREEN=0; break;
        case RED:    EWLT_RED=1; EWLT_GREEN=0; break;
        case GREEN:  EWLT_RED=0; EWLT_GREEN=1; break;
        case YELLOW: EWLT_RED=1; EWLT_GREEN=1; break;
    }
}

// ====================== BUZZER ======================
void Activate_Buzzer(void)
{
    PR2 = 0b11111001;
    T2CON = 0b00000101;
    CCPR2L = 0b01001010;
    CCP2CON = 0b00111100;
}

void Deactivate_Buzzer(void)
{
    CCP2CON = 0x00;
    LATCbits.LATC1 = 0;
}

void Wait_One_Second_With_Beep(void)
{
    SEC_LED = 1;
    Activate_Buzzer();
    Wait_Half_Second();
    SEC_LED = 0;
    Deactivate_Buzzer();
    Wait_Half_Second();
}

// ====================== PEDESTRIAN CONTROL ======================
// Direction: 0 = NS, 1 = EW
void PED_Control(char Direction, char Num_Sec)
{
    char i;
    if(Direction==0){
        // Turn OFF lower digit, show countdown on upper digit
        for(i=Num_Sec-1; i>0; i--){
            // Display i on upper digit (implement according to your 7seg wiring)
            // Example: LATx = seg_pattern[i];
            Wait_One_Second_With_Beep();
        }
        // Turn off display
    }else{
        // Turn OFF upper digit, show countdown on lower digit
        for(i=Num_Sec-1; i>0; i--){
            // Display i on lower digit
            Wait_One_Second_With_Beep();
        }
        // Turn off display
    }
}

// ====================== MAIN TRAFFIC MODES ======================
void Night_Mode(void)
{
    MODE_LED = 0; // off

    // Step1: Set all RED except NS GREEN
    Set_EW(RED); Set_EWLT(RED); Set_NSLT(RED); Set_NS(GREEN);
    Wait_N_Seconds(6);
    Set_NS(YELLOW); Wait_N_Seconds(3);
    Set_NS(RED);

    if(EWLT_SW){
        Set_EWLT(GREEN); Wait_N_Seconds(7);
        Set_EWLT(YELLOW); Wait_N_Seconds(3);
        Set_EWLT(RED);
    }

    Set_EW(GREEN); Wait_N_Seconds(6);
    Set_EW(YELLOW); Wait_N_Seconds(3);
    Set_EW(RED);

    if(NSLT_SW){
        Set_NSLT(GREEN); Wait_N_Seconds(8);
        Set_NSLT(YELLOW); Wait_N_Seconds(3);
        Set_NSLT(RED);
    }
}

void Day_Mode(void)
{
    MODE_LED = 1; // on

    Set_EW(RED); Set_EWLT(RED); Set_NSLT(RED); Set_NS(GREEN);

    if(NSPED_SW) PED_Control(0,8);
    Wait_N_Seconds(7);
    Set_NS(YELLOW); Wait_N_Seconds(3);
    Set_NS(RED);

    if(EWLT_SW){
        Set_EWLT(GREEN); Wait_N_Seconds(8);
        Set_EWLT(YELLOW); Wait_N_Seconds(3);
        Set_EWLT(RED);
    }

    Set_EW(GREEN);
    if(EWPED_SW) PED_Control(1,7);
    Wait_N_Seconds(6);
    Set_EW(YELLOW); Wait_N_Seconds(3);
    Set_EW(RED);

    if(NSLT_SW){
        Set_NSLT(GREEN); Wait_N_Seconds(7);
        Set_NSLT(YELLOW); Wait_N_Seconds(3);
        Set_NSLT(RED);
    }
}

// ====================== MAIN ======================
void main(void)
{
    OSCCON = 0x70; // 8 MHz
    TRISA = 0xFF;  // PORTA inputs
    TRISB = 0x00;
    TRISC = 0x00;
    TRISD = 0x00;

    ADCON1 = 0x0E; // RA0 analog, rest digital

    while(1)
    {
        if(MODE_SENSOR <= 2) // Example: analog threshold ~2.5V
            Day_Mode();
        else
            Night_Mode();
    }
}
