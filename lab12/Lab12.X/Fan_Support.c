#include <p18f4620.h>
#include "Main.h"
#include "Fan_Support.h"
#include "stdio.h"

char duty_cycle;
char FAN;

int get_RPM()
{
    int RPS = TMR3L / 2; // read the count. Since there are 2 pulses per rev
    // then RPS = count /2
    TMR3L = 0; // clear out the count
    return (RPS * 60); // return RPM = 60 * RPS
}

void Toggle_Fan()
{
    if(FAN == 0)
    {
        Turn_On_Fan();
    }
    else
    {
        Turn_Off_Fan();
    }
}

void Turn_Off_Fan()
{
    FAN = 0;
    FAN_EN = 0;
    FANON_LED = 0;
}

void Turn_On_Fan()
{
    FAN = 1;
    do_update_pwm(duty_cycle) ;
    FAN_EN = 1;
    FANON_LED = 1;
}


void Increase_Speed()                   // increases the speed of the fan by 5
{
   if (duty_cycle == 100) {             // if duty cycle is at 100 beep twice and stay at 100
        Do_Beep();
        Do_Beep();
        do_update_pwm(duty_cycle);
    }
    else {
        duty_cycle += 5;
        do_update_pwm(duty_cycle);
    }
   
}

void Decrease_Speed()
{
    if(duty_cycle == 0)
    {
        Do_Beep();
        Do_Beep();
        do_update_pwm(duty_cycle) ;
    }
    else{
        duty_cycle -=5;
        do_update_pwm(duty_cycle) ;
    }
}


void do_update_pwm(char duty)
{
    float dc_f;
    int dc_I;
    PR2 = 0b00000100 ; // set the frequency for 25 Khz
    T2CON = 0b00000111 ; //
    dc_f = ( 4.0 * duty / 20.0) ; // calculate factor of duty cycle versus a 25 Khz
    // signal
    dc_I = (int) dc_f; // get the integer part
    if (dc_I > duty) dc_I++; // round up function
    CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100;
    CCPR1L = (dc_I) >> 2;
} 
void Set_RGB_Color_D1(colors c)
{
    switch(c)
    {
        case OFF:     LED_R_D1=0; LED_G_D1=0; LED_B_D1=0; break;
        case RED:     LED_R_D1=1; LED_G_D1=0; LED_B_D1=0; break;
        case GREEN:   LED_R_D1=0; LED_G_D1=1; LED_B_D1=0; break;
        case YELLOW:  LED_R_D1=1; LED_G_D1=1; LED_B_D1=0; break;
        case BLUE:    LED_R_D1=0; LED_G_D1=0; LED_B_D1=1; break;
        case MAGENTA: LED_R_D1=1; LED_G_D1=0; LED_B_D1=1; break;
        case CYAN:    LED_R_D1=0; LED_G_D1=1; LED_B_D1=1; break;
        case WHITE:   LED_R_D1=1; LED_G_D1=1; LED_B_D1=1; break;
    }
}

void Set_RGB_Color_D2(colors c)
{
    switch(c)
    { 
        case OFF:     LED_R_D2=0; LED_G_D2=0; LED_B_D2=0; break;
        case RED:     LED_R_D2=1; LED_G_D2=0; LED_B_D2=0; break;
        case GREEN:   LED_R_D2=0; LED_G_D2=1; LED_B_D2=0; break;
        case YELLOW:  LED_R_D2=1; LED_G_D2=1; LED_B_D2=0; break;
        case BLUE:    LED_R_D2=0; LED_G_D2=0; LED_B_D2=1; break;
        case MAGENTA: LED_R_D2=1; LED_G_D2=0; LED_B_D2=1; break;
        case CYAN:    LED_R_D2=0; LED_G_D2=1; LED_B_D2=1; break;
        case WHITE:   LED_R_D2=1; LED_G_D2=1; LED_B_D2=1; break;
    }
}
const colors ColorTable[8] =
{
    OFF, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE
};


void Set_DC_RGB(int duty_cycle)
{
    unsigned char idx = duty_cycle / 10;
    if (idx > 7) idx = 7;
    Set_RGB_Color_D1(ColorTable[idx]);
}

void Set_RPM_RGB(int rpm)
{
    unsigned char idx = rpm / 500;
    if (idx > 7) idx = 7;
    Set_RGB_Color_D2(ColorTable[idx]);
}


