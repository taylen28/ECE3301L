#include <p18f4620.h>
#include <stdio.h>

//================ Configuration bits =================
#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF

//================ UART ===============================
void init_UART(void) {
    OSCCON = 0x60;        // 4 MHz internal oscillator
    TRISCbits.TRISC6 = 0; // TX pin as output
    TRISCbits.TRISC7 = 1; // RX pin as input

    TXSTAbits.SYNC = 0;   // Asynchronous
    TXSTAbits.BRGH = 1;   // High speed
    RCSTAbits.SPEN = 1;   // Enable serial port
    RCSTAbits.CREN = 1;   // Enable receiver
    TXSTAbits.TXEN = 1;   // Enable transmitter

    SPBRG = 25;           // 9600 baud @4MHz
    SPBRGH = 0;
}
void putch(char c) { while(!PIR1bits.TXIF); TXREG = c; }

//================ ADC ================================
void INIT_ADC(void) {
    ADCON0 = 0x01;  // AN0 selected, ADC on
    ADCON1 = 0x0E;  // AN0/AN1 analog, Vref=VDD/VSS
    ADCON2 = 0xA9;  // Right justify, Tacq=8Tad, Tad=Fosc/8
}
void SET_ADCON0(char ch) { ADCON0 = (ch<<2) | 0x01; }
unsigned int GET_FULL_ADC(void) {
    ADCON0bits.GO = 1;
    while(ADCON0bits.DONE);
    return ((unsigned int)ADRESH<<8) + ADRESL;
}

//================ TRIS ===============================
void INIT_TRIS(void) {
    TRISA = 0xFF;  // RA0, RA1 inputs
    TRISB = 0x00;  // 7-seg lower (ones)
    TRISC = 0x00;  // D1 & D2 RGB outputs
    TRISD = 0x00;  // 7-seg upper (tens)
    TRISE = 0x00;  // D3 RGB outputs
}

//================ LED DEFINES ========================
// all LEDs common cathode: 1 = ON
#define LED_ON   1
#define LED_OFF  0

// D1: RC0=RED, RC1=GREEN, RC2=BLUE
#define D1_RED   PORTCbits.RC0
#define D1_GREEN PORTCbits.RC1
#define D1_BLUE  PORTCbits.RC2

// D2: RC3=RED, RC4=GREEN, RC5=BLUE
#define D2_RED   LATCbits.LATC3
#define D2_GREEN LATCbits.LATC4
#define D2_BLUE  LATCbits.LATC5

// D3: RE0=RED, RE1=GREEN, RE2=BLUE
#define D3_RED   LATEbits.LATE0
#define D3_GREEN LATEbits.LATE1
#define D3_BLUE  LATEbits.LATE2

#define SET_RGB(LED, color) \
    do { \
        LED##_RED   = (color >> 2) & 1; \
        LED##_GREEN = (color >> 1) & 1; \
        LED##_BLUE  = (color >> 0) & 1; \
    } while(0)

//================ Delay ==============================
void Delay_One_Sec(void){ for(long j=0;j<20000;j++); }

//================ 7-Segment ==========================
const unsigned char segmap[10] =
{0x01,0x4F,0x12,0x06,0x4C,0x24,0x20,0x0F,0x00,0x04};

void DISP_LeftDigit(unsigned char d){ LATD = segmap[d]; }  // tens
void DISP_RightDigit(unsigned char d){ LATB = segmap[d]; } // ones

//================ LED Color Logic ====================
// ---- D1 compact ?5 lines ----
// bit2=R, bit1=G, bit0=B  (RC0=R, RC1=G, RC2=B)
void set_D1(int tempF){
    const unsigned char temp_map[] = {
        0b000, // <10
        0b100, // 10-19 red
        0b010, // 20-29 green
        0b110, // 30-39 yellow
        0b001, // 40-49 blue
        0b101, // 50-59 purple
        0b011, // 60-69 cyan
        0b111  // >=70 white
    };
    unsigned char idx = (tempF<70) ? tempF/10 : 7;
    SET_RGB(D1, temp_map[idx]);
}

// ---- D2 from lab table ----
void set_D2(int t){
    unsigned char c;
    if(t<35) c=0b000;
    else if(t<=45) c=0b001;
    else if(t<=55) c=0b010;
    else if(t<=65) c=0b011;
    else if(t<=75) c=0b100;
    else if(t<=78) c=0b101;
    else if(t<=82) c=0b110;
    else c=0b111;
    D2_RED   = c & 1;
    D2_GREEN = (c>>1)&1;
    D2_BLUE  = (c>>2)&1;
}

// ---- D3 from lab table ----
void set_D3(float mv){
    if(mv<2600)      {D3_RED=LED_ON; D3_GREEN=LED_ON; D3_BLUE=LED_ON;}
    else if(mv<2900) {D3_RED=LED_OFF;D3_GREEN=LED_ON; D3_BLUE=LED_ON;}
    else if(mv<3200) {D3_RED=LED_ON; D3_GREEN=LED_ON; D3_BLUE=LED_OFF;}
    else if(mv<3600) {D3_RED=LED_ON; D3_GREEN=LED_OFF;D3_BLUE=LED_OFF;}
    else             {D3_RED=LED_OFF;D3_GREEN=LED_OFF;D3_BLUE=LED_OFF;}
}

//================ MAIN ===============================
void main(void){
    init_UART();
    INIT_ADC();
    INIT_TRIS();

    while(1){
        // ---- Temperature ----
        SET_ADCON0(0);
        unsigned int steps = GET_FULL_ADC();
        float mv = (steps*5000.0)/1023.0;
        float tC = (1035.0 - mv)/5.5;
        float tF = tC*1.8 + 32.0;
        int tempF = (int)tF;
        if(tempF<0) tempF=0;

        DISP_LeftDigit(tempF/10);
        DISP_RightDigit(tempF%10);

        set_D1(tempF);
        set_D2(tempF);

        // ---- Light ----
        SET_ADCON0(1);
        unsigned int lsteps = GET_FULL_ADC();
        float lmv = (lsteps*5000.0)/1023.0;
        set_D3(lmv);

        // ---- Serial print ----
        printf("Temp=%d F  V=%.1f mV  Light=%.1f mV\r\n", tempF, mv, lmv);

        Delay_One_Sec();
    }
}
