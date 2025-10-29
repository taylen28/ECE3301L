#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <string.h>

#include "main.h"
#include "utils.h"
#include "Main_Screen.h"
#include "ST7735_TFT.h"

void Set_NS(char color);
void Set_NS_LT(char color);
void Set_EW(char color);
void Set_EW_LT(char color);
void PED_Control(char Direction, char Num_Sec);
void Day_Mode();
void Night_Mode();
void Wait_N_Seconds(char);
void Wait_One_Second_With_Beep();
void Wait_One_Second();
void Do_Flashing();


#define _XTAL_FREQ  8000000             // Set operation for 8 Mhz
#define TMR_CLOCK   _XTAL_FREQ/4        // Timer Clock 2 Mhz

char dir;
char Count; // RAM variable for Second Count
char PED_Count; // RAM variable for Second Pedestrian Count

char MODE;
char direction;
float volt;

extern char Light_Sensor;

char INT0_Flag;
char INT1_Flag;
char INT2_Flag;

char NS_PED_SW = 0;
char EW_PED_SW = 0;

char Flashing_Request = 0;
char Flashing_Status = 0; 

void main(void) {
    OSCCON = 0x70; // set the system clock to be 1MHz 1/4 of the 4MHz
    TRISA = 0b00111001; //inputs RA0/AN0-RA4 inputs,RA5 output
    TRISB = 0x07; // outputs
    TRISC = 0x01; // set PORTC as outputs
    TRISD = 0x00; // set PORTD as outputs
    TRISE = 0x00; // set PORTE as outputs
    Init_ADC();
    Init_UART();
    
    PORTD = 0x00;

    Initialize_LCD_Screen(); // Initialize the TFT screen

    volt = Read_Volt(0); // 

    Light_Sensor = volt < 2.5 ? 1 : 0; // Mode = 1, Day_mode, Mode = 0 Night_mode

    while (1) // forever loop
    {

        if (Light_Sensor == 1) {
            MODE = 1;
            Day_Mode(); // calls Day_Mode() function
        } else {
            MODE = 0;
            Night_Mode(); // calls Day_Mode() function
        }
        
        if (Flashing_Request == 1) {
            Flashing_Request = 0;
            Flashing_Status = 1;
            Do_Flashing(); 
        }

    }
}

void Set_NS(char color) {
    direction = NS;
    update_LCD_color(direction, color);

    switch (color) {
        case OFF: NS_RED = 0;
            NS_GREEN = 0;
            break; // Turns off the NS LED
        case RED: NS_RED = 1;
            NS_GREEN = 0;
            break; // Sets NS LED RED
        case GREEN: NS_RED = 0;
            NS_GREEN = 1;
            break; // sets NS LED GREEN
        case YELLOW: NS_RED = 1;
            NS_GREEN = 1;
            break; // sets NS LED YELLOW
    }
    // add code here
}

void Set_NS_LT(char color) {
    direction = NSLT;
    update_LCD_color(direction, color);

    switch (color) {
        case OFF: NSLT_RED = 0;
            NSLT_GREEN = 0;
            break; // Turns off the NS LED
        case RED: NSLT_RED = 1;
            NSLT_GREEN = 0;
            break; // Sets NS LED RED
        case GREEN: NSLT_RED = 0;
            NSLT_GREEN = 1;
            break; // sets NS LED GREEN
        case YELLOW: NSLT_RED = 1;
            NSLT_GREEN = 1;
            break; // sets NS LED YELLOWf
    }
    // add code here
}

void Set_EW(char color) {
    direction = EW;
    update_LCD_color(direction, color);

    switch (color) {
        case OFF: EW_RED = 0;
            EW_GREEN = 0;
            break; // Turns off the NS LED
        case RED: EW_RED = 1;
            EW_GREEN = 0;
            break; // Sets NS LED RED
        case GREEN: EW_RED = 0;
            EW_GREEN = 1;
            break; // sets NS LED GREEN
        case YELLOW: EW_RED = 1;
            EW_GREEN = 1;
            break; // sets NS LED YELLOW
    }
    // add code here
}

void Set_EW_LT(char color) {
    direction = EWLT;
    update_LCD_color(direction, color);

    switch (color) {
        case OFF: EWLT_RED = 0;
            EWLT_GREEN = 0;
            break; // Turns off the NS LED
        case RED: EWLT_RED = 1;
            EWLT_GREEN = 0;
            break; // Sets NS LED RED
        case GREEN: EWLT_RED = 0;
            EWLT_GREEN = 1;
            break; // sets NS LED GREEN
        case YELLOW: EWLT_RED = 1;
            EWLT_GREEN = 1;
            break; // sets NS LED YELLOW
    }
    // add code here
}

void PED_Control(char direction, char Num_Sec) {
    
//    update_LCD_PED_Count(direction, i);
    for (int i = Num_Sec-1; i >= 0; i--) {
        
        update_LCD_PED_Count(direction, i);
        Wait_One_Second_With_Beep();
    }
    
    switch(direction){
        case NS:
            NS_PED_SW = 0;
            break;
        case EW:
            EW_PED_SW = 0;
            break;
    }
}

void Day_Mode() {
    Set_EW(RED); //Step 1
    Set_EW_LT(RED);
    Set_NS_LT(RED);
    Set_NS(GREEN);

    if (NS_PED_SW == 1) //STEP2
    {
        PED_Control(NS, PEDESTRIAN_NS_WAIT);

    }
    Wait_N_Seconds(7); //STEP3
    Set_NS(YELLOW); //STEP 4
    Wait_N_Seconds(3);
    Set_NS(RED); //STEP5

    if (EW_LT_SW == 1) //STEP6
    {
        Set_EW_LT(GREEN); //STEP 7
        Wait_N_Seconds(8);
        Set_EW_LT(YELLOW); //8
        Wait_N_Seconds(3);
        Set_EW_LT(RED); //9

    }
    Set_EW(GREEN); //10
    if (EW_PED_SW == 1) {
        PED_Control(EW, PEDESTRIAN_EW_WAIT);
    }
    Set_EW(GREEN); //11
    Wait_N_Seconds(6);
    Set_EW(YELLOW); //12
    Wait_N_Seconds(3);
    Set_EW(RED); //13
    if (NS_LT_SW == 1) //14
    {
        Set_NS_LT(GREEN); //15
        Wait_N_Seconds(7);
        Set_NS_LT(YELLOW); //16
        Wait_N_Seconds(3);
        Set_NS_LT(RED); //17
    }
}

void Night_Mode() {
    // STEP 1
    NS_PED_SW = 0;
    EW_PED_SW = 0;
    
    Set_EW(RED);
    Set_EW_LT(RED);
    Set_NS_LT(RED);
    Set_NS(GREEN);

    // STEP 2
    Wait_N_Seconds(6);

    // STEP 3
    Set_NS(YELLOW);
    Wait_N_Seconds(3);

    // STEP 4
    Set_NS(RED);

    // STEP 5
    if (EW_LT_SW == 1) {

        // STEP 6
        Set_EW_LT(GREEN);
        Wait_N_Seconds(7);

        // STEP 7
        Set_EW_LT(YELLOW);
        Wait_N_Seconds(3);

        // STEP 8
        Set_EW_LT(RED);


    }

    // STEP 9
    Set_EW(GREEN);
    Wait_N_Seconds(6);

    // STEP 10
    Set_EW(YELLOW);
    Wait_N_Seconds(3);

    // STEP 11
    Set_EW(RED);

    //
    if (NS_LT_SW == 1) {

        // STEP 6
        Set_NS_LT(GREEN);
        Wait_N_Seconds(8);

        // STEP 7
        Set_NS_LT(YELLOW);
        Wait_N_Seconds(3);

        // STEP 8
        Set_EW_LT(RED);

    }
}

void Wait_One_Second() //creates one second delay and blinking asterisk
{
    SEC_LED = 1;
    Draw_Star();
    Wait_Half_Second(); // Wait for half second (or 500 msec)
    SEC_LED = 0;
    Erase_Star();
    Wait_Half_Second(); // Wait for half second (or 500 msec)
    update_LCD_misc();
    
}

void Wait_One_Second_With_Beep() //creates one second delay as well as sound buzzer
{
    SEC_LED = 1;
    Draw_Star();
    Activate_Buzzer();
    Wait_Half_Second(); // Wait for half second (or 500 msec)

    SEC_LED = 0;
    Erase_Star();
    Deactivate_Buzzer();
    Wait_Half_Second(); // Wait for half second (or 500 msec)
    update_LCD_misc();
}

void Wait_N_Seconds(char seconds) {
    char I;
    for (I = seconds; I > 0; I--) {
        update_LCD_count(direction, I);// add code here;
        Wait_One_Second(); // calls Wait_One_Second for x number of times

    }
    update_LCD_count(direction, 0);// add code here;
}

void Do_Flashing(){
    while(Flashing_Status) {
        switch (Flashing_Request){
            case 0:
                Set_EW(RED);
                Set_EW_LT(RED);
                Set_NS_LT(RED);
                Set_NS(RED);
                Wait_One_Second();
                Set_EW(OFF);
                Set_EW_LT(OFF);
                Set_NS_LT(OFF);
                Set_NS(OFF);
                Wait_One_Second();
                break;
            case 1:
                Flashing_Request = 0;
                Flashing_Status = 0;
                break;
        }
    }
}