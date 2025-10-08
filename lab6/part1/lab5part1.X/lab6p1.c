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
#define DP PORTDbits.RD7

// Array for 7-segment display digits (0-9)
const char LED_7seg[10] = {0x01, 0x4F, 0x12, 0x06, 0x4C, 0x24, 0x20, 0x0F, 0x00, 0x04};

/*************************************************************/ 
/*                          Prototype Area                   */ 
/*************************************************************/
void putch(char c);
void Init_UART(void);
void Init_ADC(void);
void Init_TRIS(void);
void Select_ADC_Channel(char channel);
unsigned int get_full_ADC(void);
void Display_7Seg(float voltage);
void set_D1_color(float resistance);
void set_D2_color(float resistance);

void Activate_Buzzer(void);
void Deactivate_Buzzer(void);

void SET_D1_RED(void);
void SET_D1_GREEN(void);
void SET_D1_BLUE(void);
void SET_D1_YELLOW(void);
void SET_D1_CYAN(void);
void SET_D1_PURPLE(void);
void SET_D1_WHITE(void);
void SET_D1_OFF(void);

void SET_D2_RED(void);
void SET_D2_GREEN(void);
void SET_D2_BLUE(void);
void SET_D2_YELLOW(void);
void SET_D2_CYAN(void);
void SET_D2_PURPLE(void);
void SET_D2_WHITE(void);
void SET_D2_OFF(void);

void Delay_One_Sec(void);

/****************************************************************************/ 
/*                         putch(char): Function to output text to Tera Term */
/****************************************************************************/
void putch(char c) {
    while (!TRMT);   // Wait for transmit register to be empty
    TXREG = c;       // Load the character into the transmit register
}

/*****************************************************************/ 
/*                      Init_UART(): Function to initialize UART */
/*****************************************************************/
void Init_UART() {
    OpenUSART(USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE &
              USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25);
    OSCCON = 0x60;   // Configure internal oscillator to 4 MHz
}

/***************************************************************/ 
/*                      Init_ADC(): Function to initialize ADC */
/***************************************************************/
void Init_ADC() {
    ADCON0 = 0x01;   // Select AN0, turn ADC on
    ADCON1 = 0x1A;   // Configure AN0-AN3 as analog, others digital
    ADCON2 = 0xA9;   // Right justified
}

/****************************************************************************/ 
/*                    Init_TRIS(void): Function to initialize TRIS registers*/
/****************************************************************************/
void Init_TRIS(void) {
    TRISA = 0xFF;    // Sets Port A to input 
    TRISB = 0x00;    // Sets Port B to output 
    TRISC = 0x00;    // Sets Port C to output 
    TRISD = 0x00;    // Sets Port D to output 
    TRISE = 0x00;    // Sets Port E to output 
}

/**********************************************************************************/ 
/*                Select_ADC_Channel(char): Select ADC channel for reading voltage */
/**********************************************************************************/
void Select_ADC_Channel(char channel) {
    ADCON0 = channel * 4 + 1;   // Set ADC channel and enable ADC
}

/**********************************************************************/ 
/*                     get_full_ADC(void): Read ADC value from channel */
/**********************************************************************/
unsigned int get_full_ADC(void) {
    int result;	
    ADCON0bits.GO = 1;           // Start ADC conversion
    while (ADCON0bits.DONE);     // Wait for conversion to complete
    result = (ADRESH * 0x100) + ADRESL;  // Combine high and low bytes
    return result;
}

/**********************************************************************/ 
/*                   Display_7Seg(float): Display voltage on 7-segment */
/**********************************************************************/
void Display_7Seg(float voltage) {
    int upper = (int) voltage;               // Extract whole number part
    int lower = (int) ((voltage - upper) * 10); // Extract decimal part
    PORTD = LED_7seg[upper];   // Display upper digit
    PORTB = LED_7seg[lower];   // Display lower digit
}

/*************************************************************/ 
/*             set_D1_color(float): Set RGB LED D1 color      */
/*************************************************************/
void set_D1_color(float resistance) {
    int range = (int) (resistance / 10); // Get resistance range in tens
    if (range >= 0 && range < 7)  
        PORTE = range;            // Output corresponding color
    else
        PORTE = 0x07;             // Display white for 70k+ resistance
}

/*************************************************************/ 
/*             set_D2_color(float): Set RGB LED D2 color      */
/*************************************************************/
void set_D2_color(float resistance) 
{
    if (resistance > 0.3) {
        SET_D2_OFF();    // Greater than 0.3K - Turn off LED
    } else if (resistance >= 0.2 && resistance <= 0.3) {
        SET_D2_RED();    // 0.2K - 0.3K - Red
    } else if (resistance >= 0.1 && resistance < 0.2) {
        SET_D2_GREEN();  // 0.1K - 0.2K - Green
    } else if (resistance >= 0.07 && resistance < 0.1) {
        SET_D2_BLUE();   // 0.07K - 0.1K - Blue
    } else {
        SET_D2_WHITE();  // Less than 0.07K - White
    }
}

/*************************************************************/ 
/*                Activate_Buzzer(): Turn on the buzzer       */
/*************************************************************/
void Activate_Buzzer(void) {
    PR2 = 0b11111001;
    T2CON = 0b00000101;
    CCPR2L = 0b01001010;
    CCP2CON = 0b00111100;
}

/*************************************************************/ 
/*                Deactivate_Buzzer(): Turn off the buzzer    */
/*************************************************************/
void Deactivate_Buzzer(void) {
    CCP2CON = 0x0;
    PORTCbits.RC1 = 0;
}

/************************************************************************/ 
/*                SET_D1_RGB & SET_D2_RGB color function: Activate ports*/
/************************************************************************/
void SET_D1_RED()    { D1_RED = 1; D1_GREEN = 0; D1_BLUE = 0; }
void SET_D1_GREEN()  { D1_RED = 0; D1_GREEN = 1; D1_BLUE = 0; }
void SET_D1_BLUE()   { D1_RED = 0; D1_GREEN = 0; D1_BLUE = 1; }
void SET_D1_YELLOW() { D1_RED = 1; D1_GREEN = 1; D1_BLUE = 0; }
void SET_D1_CYAN()   { D1_RED = 0; D1_GREEN = 1; D1_BLUE = 1; }
void SET_D1_PURPLE() { D1_RED = 1; D1_GREEN = 0; D1_BLUE = 1; }
void SET_D1_WHITE()  { D1_RED = 1; D1_GREEN = 1; D1_BLUE = 1; }
void SET_D1_OFF()    { D1_RED = 0; D1_GREEN = 0; D1_BLUE = 0; }

void SET_D2_RED()    { D2_RED = 1; D2_GREEN = 0; D2_BLUE = 0; }
void SET_D2_GREEN()  { D2_RED = 0; D2_GREEN = 1; D2_BLUE = 0; }
void SET_D2_BLUE()   { D2_RED = 0; D2_GREEN = 0; D2_BLUE = 1; }
void SET_D2_YELLOW() { D2_RED = 1; D2_GREEN = 1; D2_BLUE = 0; }
void SET_D2_CYAN()   { D2_RED = 0; D2_GREEN = 1; D2_BLUE = 1; }
void SET_D2_PURPLE() { D2_RED = 1; D2_GREEN = 0; D2_BLUE = 1; }
void SET_D2_WHITE()  { D2_RED = 1; D2_GREEN = 1; D2_BLUE = 1; }
void SET_D2_OFF()    { D2_RED = 0; D2_GREEN = 0; D2_BLUE = 0; }

/*******************************************************************/ 
/*                     Delay_One_Sec: Creates a delay of one second */
/*******************************************************************/
void Delay_One_Sec() {
    for (int i = 0; i < delay; i++);   // Delay loop
}

/*************************************************************/ 
/*                           Main Function                    */
/*************************************************************/
void main() {
    Init_UART();     // Initialize UART
    Init_ADC();      // Initialize ADC
    Init_TRIS();     // Initialize TRIS settings
    float Rref = 22;  // Reference resistor (22K ohms)
    Select_ADC_Channel(4);  // Select AN4 for measurement
    
    while (1) 
    { 
        int num_step = get_full_ADC();
        float volt = (num_step * 5.0)/1024;
        float R = (volt / (5.0 - volt)) * Rref;
        if (R < 10)
        {
            int upper = R;
            int lower = (int) ((R - upper) * 10);
            PORTD = LED_7seg[upper];
            PORTB = LED_7seg[lower];
            DP = 0;
        }
        if (R >= 10)
        {
            int upper = R / 10;
            int lower = (int) (R) % 10;
            PORTD = LED_7seg[upper];
            PORTB = LED_7seg[lower];
            DP = 1;
        }
        printf("Resistance: %f K\r\n\n", R);
        set_D1_color(R);
        set_D2_color(R);
        if (R < 0.07) Activate_Buzzer(); else Deactivate_Buzzer();
        Delay_One_Sec();
    }
}