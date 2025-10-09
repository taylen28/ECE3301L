#include <p18f4620.h>
#include <stdio.h>
#include <math.h>
#include <usart.h>
#pragma config OSC = INTIO67
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOREN=OFF

//Define RGB LED pins for three LEDs
#define D1_RED PORTCbits.RC3
#define D1_GREEN PORTCbits.RC4
#define D1_BLUE PORTCbits.RC5

#define D2_RED PORTCbits.RC0
#define D2_GREEN PORTCbits.RC1
#define D2_BLUE PORTCbits.RC2

#define D3_RED PORTEbits.RE0
#define D3_GREEN PORTEbits.RE1
#define D3_BLUE PORTEbits.RE2

//Array to store 7 segment display values for digits 0-9
char LED_7seg[] = {0x01, 0x4F, 0x12, 0x06, 0x4C, 0x24, 0x20, 0x0F, 0x00, 0x0C};

//Initializes UART communication
void Init_UART() {
    OpenUSART(USART_TX_INT_OFF & USART_RX_INT_OFF &
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
    USART_BRGH_HIGH, 25);
    OSCCON = 0x60;
}

//Function to transmit a character via UART
void putch(char c) {
    while (!TRMT);
    TXREG = c;
}

//Configures input/output directions for microcontroller ports
void Init_ADC(void) {
    ADCON0 = 0x01 * 4 + 1;
    ADCON1 = 0x0E;
    ADCON2 = 0xA9;
}

void Init_TRIS(void) {
    TRISA = 0xFF; // sets PORTA as input
    TRISB = 0x00; // sets PORTB as output
    TRISC = 0x00; // sets PORTC as output
    TRISD = 0x00; // sets PORTD as output
    TRISE = 0x00; // sets PORTE as output
}

//Reads a full ADC conversion result
unsigned int get_full_ADC(void) {
    int result;
    ADCON0bits.GO = 1;
    while (ADCON0bits.DONE == 1);
    result = (ADRESH * 0x100) + ADRESL;
    return result;
}

//Sets ADCON0 to read from a specified analog channel
void Set_ADCON0(char AN_pin) {
    ADCON0 = AN_pin * 4 + 1;
}

//Control RGB LED D1 based on temperature value
void SET_RGB_D1(int temp_up) {
    if(temp_up >= 7)
        {PORTC = (7<<3);}
        else
        {PORTC = (temp_up<<3);}
}

//Control RGB LED D2 based on temperature in Fahrenheit
void SET_RGB_D2(int temp_f) {
    if (temp_f < 35) PORTC &= 0x18;
    else if (temp_f < 45) PORTC |= 0x01; 
    else if (temp_f < 55) PORTC |= 0x02;
    else if (temp_f < 65) PORTC |= 0x03;
    else if (temp_f < 75) PORTC |= 0x04;
    else if (temp_f < 78) PORTC |= 0x05;
    else if (temp_f < 82) PORTC |= 0x06;
    else PORTC |= 0x07;
}

//Control RGB LED D3 based on voltage reading
void SET_RGB_D3(float voltage) {
    if (voltage < 2600) { D3_RED = 1; D3_GREEN = 1; D3_BLUE = 1; } //white
    else if (voltage < 2900) { D3_RED = 0; D3_GREEN = 1; D3_BLUE = 1; } 
    else if (voltage < 3200) { D3_RED = 1; D3_GREEN = 1; D3_BLUE = 0; }
    else if (voltage < 3600) { D3_RED = 1; D3_GREEN = 0; D3_BLUE = 0; }
    else { D3_RED = 0; D3_GREEN = 0; D3_BLUE = 0; }
}

//Implements a software delay of approximately 1 second
void WAIT_1_SEC() {
    for (int j = 0; j < 27000; j++);
}

//Main function to execute microcontroller program
void main() {
    Init_UART();
    Init_ADC();
    Init_TRIS();
    while (1) {
        Set_ADCON0(0); // select ADC channel 0
        int mVolt_in = get_full_ADC();
        float mVolt_in_float = mVolt_in * 4.0;
        float temp_Celsius_float = (1035.0 - mVolt_in_float) / 5.50;
        float temp_Fahren_float = (1.80 * temp_Celsius_float) + 32.0;
        int temp_Fahren = (int)temp_Fahren_float;
        char UPPER_DIS = temp_Fahren / 10;
        char LOWER_DIS = temp_Fahren % 10;
        printf("Temperature: %dF\r\nVoltage: %d mV\r\n", temp_Fahren, (int)mVolt_in_float);
        PORTB = LED_7seg[LOWER_DIS]; //Display lower digit on 7 segment display
        PORTD = LED_7seg[UPPER_DIS]; //Display upper digit on 7 segment display
        SET_RGB_D1(temp_Fahren); //set LED D1 color based on temperature
        SET_RGB_D2(temp_Fahren); //set LED D2 color based on temperature
        Set_ADCON0(1); //Select ADC channel 1
        int mVolt_in_1 = get_full_ADC() * 4;
        SET_RGB_D3(mVolt_in_1); //Set LED D3 color based on voltage
        printf("Photo Resistor Voltage: %d mV\r\n\r\n", mVolt_in_1);
        WAIT_1_SEC(); //wait for 1 second
    }
}