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
void Set_NSLT(char color);
void Set_EW(char color);
void Set_EWLT(char color);
void PED_Control( char Direction, char Num_Sec);
void Day_Mode();
void Night_Mode();
void Wait_N_Seconds (char);
void Wait_One_Second_With_Beep();
void Wait_One_Second();
void Do_Flashing();

char INT0_Flag;
char INT1_Flag;
char INT2_Flag; 


#define _XTAL_FREQ  8000000             // Set operation for 8 Mhz
#define TMR_CLOCK   _XTAL_FREQ/4        // Timer Clock 2 Mhz

char dir;
char Count;                             // RAM variable for Second Count
char PED_Count;                         // RAM variable for Second Pedestrian Count

char  MODE;
char direction;
float volt;

extern char Light_Sensor;

char NS_PED_SW = 0;
char EW_PED_SW = 0; 

char Flashing_Request = 0;
char Flashing_Status = 0; 

void main(void)
{
    OSCCON = 0x70;                      // set the system clock to be 1MHz 1/4 of the 4MHz
    TRISA = 0x1F;                       //inputs RA0/AN0-RA4 inputs,RA5 output
    TRISB = 0x07;                       // outputs
    TRISC = 0x00;                       // set PORTC as outputs
    TRISD = 0x00;                       // set PORTD as outputs
    TRISE = 0x00;                       // set PORTE as outputs
    Init_ADC();
    Init_UART();

    Initialize_LCD_Screen();                        // Initialize the TFT screen
    RBPU = 0;
    init_INTERRUPT();
    
    /* For Testing 
     while (1)
    { // Do nothing,
     if (INT0_Flag == 1)
    {
     INT0_Flag = 0; // clear the flag
     printf("INT0 interrupt pin detected \r\n");
    // print a message that INT0 has
    // occurred
     }
     if (INT1_Flag == 1)
    {
     INT1_Flag = 0; // clear the flag
     printf("INT1 interrupt pin detected \r\n");
    // print a message that INT1 has
    // occurred
     }
     if (INT2_Flag == 1)
    {
     INT2_Flag = 0; // clear the flag
     printf("INT2 interrupt pin detected \r\n");
    // print a message that INT2 has
    // occurred
     }
    } 
   */

    volt = Read_Volt(0);                        // 

    Light_Sensor = volt < 2.5 ? 1:0;                 // Mode = 1, Day_mode, Mode = 0 Night_mode

    while(1)                                    // forever loop
    {

        if (Light_Sensor == 1)    
        {
            Day_Mode();                         // calls Day_Mode() function
        }
        else
        {
            Night_Mode();                       // calls Night_Mode() function
        }
        if (Flashing_Request == 1) {
            Flashing_Request = 0;
            Flashing_Status = 1;
            Do_Flashing();
        } 
     
    } 
}
void Do_Flashing(){
    while(Flashing_Status == 1){
        if(Flashing_Request == 0){
            
            Set_NS(RED);
            Set_NSLT(RED);
            Set_EW(RED);
            Set_EWLT(RED);
            
            Wait_One_Second();
            
            Set_NS(OFF);
            Set_NSLT(OFF);
            Set_EW(OFF);
            Set_EWLT(OFF);
            
            Wait_One_Second();
        } else if(Flashing_Request == 1){
            Flashing_Request = 0;
            Flashing_Status  = 0;
        }
    }
}

void Set_NS(char color)
{
    // add code here
    direction = NS; 
    update_LCD_color (direction, color); 
    switch (color)
    {
    case OFF: NS_RED =0;NS_GREEN=0;break; // Turns off the NS LED
    case RED: NS_RED =1;NS_GREEN=0;break; // Sets NS LED RED
    case GREEN: NS_RED =0;NS_GREEN=1;break; // sets NS LED GREEN
    case YELLOW: NS_RED =1;NS_GREEN=1;break; // sets NS LED YELLOW
    }
}

void Set_NSLT(char color)
{
    // add code here
    direction = NSLT; 
    update_LCD_color (direction, color); 
    switch (color)
    {
    case OFF: NSLT_RED =0;NSLT_GREEN=0;break; // Turns off the NS LED
    case RED: NSLT_RED =1;NSLT_GREEN=0;break; // Sets NS LED RED
    case GREEN: NSLT_RED =0;NSLT_GREEN=1;break; // sets NS LED GREEN
    case YELLOW: NSLT_RED =1;NSLT_GREEN=1;break; // sets NS LED YELLOW
    }
}

void Set_EW(char color)
{
    // add code here
    direction = EW; 
    update_LCD_color (direction, color);
    switch (color)
    {
    case OFF: EW_RED =0;EW_GREEN=0;break; // Turns off the NS LED
    case RED: EW_RED =1;EW_GREEN=0;break; // Sets NS LED RED
    case GREEN: EW_RED =0;EW_GREEN=1;break; // sets NS LED GREEN
    case YELLOW: EW_RED =1;EW_GREEN=1;break; // sets NS LED YELLOW
    }
}

void Set_EWLT(char color)
{
    // add code here
    direction = EWLT; 
    update_LCD_color (direction, color);
    switch (color)
    {
    case OFF: EWLT_RED =0;EWLT_GREEN=0;break; // Turns off the NS LED
    case RED: EWLT_RED =1;EWLT_GREEN=0;break; // Sets NS LED RED
    case GREEN: EWLT_RED =0;EWLT_GREEN=1;break; // sets NS LED GREEN
    case YELLOW: EWLT_RED =1;EWLT_GREEN=1;break; // sets NS LED YELLOW
    }
}

void PED_Control( char direction, char Num_Sec)
{ 
    // add code here
  for(int i=Num_Sec;i>0;i--)
  {
      update_LCD_PED_Count(direction, i-1); // Display Num_Sec-1 on LED2
      Wait_One_Second_With_Beep();
  }
  if(direction){
      EW_PED_SW = 0;
  } else{
      NS_PED_SW = 0;
  }
 
}

void Day_Mode()
{
    // add code here
    MODE     = 1;           // Day
    MODE_LED = 1;           // Signify Day Mode
    Set_EW(RED);
    Set_EWLT(RED);
    Set_NSLT(RED);
    Set_NS(GREEN);          // NS Green
    if(NS_PED_SW){          // Checks for pedestrians
        PED_Control(NS,PEDESTRIAN_NS_WAIT);
    }
    Wait_N_Seconds(NS_WAIT);
    Set_NS(YELLOW);
    Wait_N_Seconds(3);
    Set_NS(RED);            // End of NS Phase
    if(EW_LT_SW){           // Checks for EW Left Turn
        Set_EWLT(GREEN);
        Wait_N_Seconds(EW_LT_WAIT);
        Set_EWLT(YELLOW);
        Wait_N_Seconds(3);
        Set_EWLT(RED);
    }
    Set_EW(GREEN);          // Start of EW
    if(EW_PED_SW){          // Checks for pedestrians
        PED_Control(EW,PEDESTRIAN_EW_WAIT);
    }
    Wait_N_Seconds(EW_WAIT);
    Set_EW(YELLOW);
    Wait_N_Seconds(3);
    Set_EW(RED);
    if(NS_LT_SW){            // Checks for NS Left Turn
        Set_NSLT(GREEN);
        Wait_N_Seconds(NS_LT_WAIT);
        Set_NSLT(YELLOW);
        Wait_N_Seconds(3);
        Set_NSLT(RED);
    }                       // End of Day Mode
}

void Night_Mode()
{ 
    // add code here
    MODE      = 0;           // Night
    MODE_LED  = 0;           // Signify Night Mode
    EW_PED_SW = 0;
    NS_PED_SW = 0;
    
    Set_EW(RED);
    Set_EWLT(RED);
    Set_NSLT(RED);
    Set_NS(GREEN);          // Start of NS Phase
    Wait_N_Seconds(NIGHT_NS_WAIT);
    Set_NS(YELLOW);
    Wait_N_Seconds(3);
    Set_NS(RED);            // End of NS
    if(EW_LT_SW){            // Checks for EW Left Turn
        Set_EWLT(GREEN);
        Wait_N_Seconds(NIGHT_EW_LT_WAIT);
        Set_EWLT(YELLOW);
        Wait_N_Seconds(3);
        Set_EWLT(RED);
    }
    Set_EW(GREEN);          // Start of EW Phase
    Wait_N_Seconds(NIGHT_EW_WAIT);
    Set_EW(YELLOW);
    Wait_N_Seconds(3);
    Set_EW(RED);            // End of EW
    if(NS_LT_SW){            // Checks for NS Left Turn
        Set_NSLT(GREEN);
        Wait_N_Seconds(NIGHT_NS_LT_WAIT);
        Set_NSLT(YELLOW);
        Wait_N_Seconds(3);
        Set_NSLT(RED);
    }
    // Finish Night Mode
}

void Wait_One_Second()							//creates one second delay and blinking asterisk
{
    SEC_LED = 1;
    Draw_Star();
    Wait_Half_Second();                         // Wait for half second (or 500 msec)
    SEC_LED = 0;
    Erase_Star();
    Wait_Half_Second();                         // Wait for half second (or 500 msec)
    update_LCD_misc();
}

void Wait_One_Second_With_Beep()				//creates one second delay as well as sound buzzer
{
    SEC_LED = 1;
    Draw_Star();
    Activate_Buzzer();
    Wait_Half_Second();                         // Wait for half second (or 500 msec)

    SEC_LED = 0;
    Erase_Star();
    Deactivate_Buzzer();
    Wait_Half_Second();                         // Wait for half second (or 500 msec)
    update_LCD_misc();
}
 

void Wait_N_Seconds (char seconds)
{
    char I;
    for (I = seconds; I> 0; I--)
    {
		// add code here;
        update_LCD_count(direction, I);
        Wait_One_Second();          			// calls Wait_One_Second for x number of times
        
    }
	// add code here;
    update_LCD_count(direction, 0);
}
  
// }






