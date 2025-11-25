#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ST7735_TFT.h"
#include "I2C_Support.h"
#include "Setup_Time.h"
#include "Main_Screen.h"
#include "Main.h"
#include "utils.h"
#include "Fan_Support.h"


extern unsigned char second, minute, hour, dow, day, month, year;
extern unsigned char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;

extern char setup_time[];
extern char setup_date[];

extern char buffer[31];
extern char *nbr;
extern char *txt;

extern char time[];
extern char date[];
extern char found;
extern char SYSTEM_ON;

char In_Time_Setup;
char Select_Field;  
extern char Timing_Txt[];
extern char tempSecond;
extern int rpm;
extern float volt;
extern signed char tempC, tempF;
extern char duty_cycle;
extern char light_flag;
extern char FAN_TIMER_TIME;

void Do_Setup_Time(void)
{

    Select_Field = 0;                           // start at field 0

    DS3231_Read_Time();                         // Read actual time
    setup_second = bcd_2_dec(second);           // Set setup time as current time
    setup_minute = bcd_2_dec(minute);           // convert all the bcd to real integer
    setup_hour = bcd_2_dec(hour);               //
    setup_day = bcd_2_dec(day);
    setup_month = bcd_2_dec(month);
    setup_year = bcd_2_dec(year);
    
    Initialize_Setup_Time_Screen();             // Initialize setup screen
    Update_Setup_Time_Screen();                 // Update screen with latest information
    In_Time_Setup = 1;                          // set variable to stay in setup mode
    
    while (In_Time_Setup == 1)
    {
        DS3231_Read_Time();


        if(tempSecond != second)
        {
            tempSecond = second;
            rpm = get_RPM();            
 
            volt = Read_Volt(0);
            if (volt > 3.000) light_flag = 1;
            tempC = DS1621_Read_Temp();
            tempF = (tempC * 9 / 5) + 32;
            Set_D1_RGB(duty_cycle);
            Set_D2_RGB(rpm);
            Set_D3_RGB(FAN_TIMER_MODE);			
            
            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
            printf (" Temp = %d C = %d F ", tempC, tempF);
            printf ("Light Sensor = %f V", volt);
            printf (" RPM = %d  dc = %d ls = %d\r\n", rpm, duty_cycle, light_flag);
            Monitor_Fan();

        }
        if (check_for_button_input() == 1)
         {

             if (found >= Prev && found <= EQ)
             {
                Do_Beep_Good(); 
                if (found == Prev) Go_Prev_Field();
                
                if (found == Next) 		// add code here to check if 'Next' then use Go_Next_Field(); ;
               
                if (found == Play_Pause)// add code here to check if 'Play_Pause' then use Do_Save_New_Time();
                if (found == EQ) 		// add code here to check if 'EQ' then use Exit_Time_Setup();
                
                if (found == Minus) 	// add code here to check if 'Minus' then use Decrease_Time();
               
                if (found == Plus)  	// add code here to check if 'Plus' then use Increase_Time();
                found = 0xff;
             }
             else
             {
                 Do_Beep_Bad();
             }
        }                   
    }
}

void Increase_Time()
{
            switch (Select_Field)               		// Increment the proper field
            {
                case 0:
                    setup_hour++;
                    if (setup_hour == 24) setup_hour = 0; 
                    break;

                case 1:
                    // add code here to handle minute increment - also check if new minute is 60. If so, make it 0

                    break;

                case 2:
                    // add code here to handle second increment - also check if new second is 60. If so, make it 0

                    break;

                case 3:
                    // add code here to handle month increment - also check if new month is 13. If so, make it 1

                    break;

                case 4:
                    // add code here to handle day increment - also check if new day is 32. If so, make it 1  

                    break;

                case 5:
                    // add code here to handle year increment - also check if new year is 100. If so, make it 0
                    setup_year++;
                    break;

                default:
                    break;
            }    
            Update_Setup_Time_Screen();         		// Update screen with latest info
}

void Decrease_Time()
{
            switch (Select_Field)               		// Decrement the proper field
            {
                case 0:
                    if (setup_hour == 0) setup_hour = 23;
                    else --setup_hour;
                    break;
                case 1:
					// add code here to check is minute is at 0. If not, decrement. If yes, if so, make it 59
                    break;

                case 2:
                    // add code here to check is second is at 0. If not, decrement. If yes, if so, make it 59

                    break;

                case 3:
                    // add code here to check is month is at 1. If not, decrement. If yes, if so, make it 12

                    break;

                case 4:
                    // add code here to check is day is at 1. If not, decrement. If yes, if so, make it 31
  
                    break;

                case 5:
                    // add code here to check is year is at 00. If not, decrement. If yes, if so, make it 99

                    break;

                default:
                break;
            }                
            Update_Setup_Time_Screen();                 // Update screen with latest info
        } 

void Go_Next_Field()
{
    Select_Field++;                                     // Move to next field
    if (Select_Field == 6) Select_Field = 0;            // Wrap around if necessary
    Update_Setup_Screen_Cursor_Forward(Select_Field);           // Update cursor

}  

void Go_Prev_Field()
{
    if (Select_Field == 0) Select_Field = 5;
    else Select_Field--;                                // Move to next field
    Update_Setup_Screen_Cursor_Backward(Select_Field);  // Update cursor
} 

void Do_Save_New_Time()
{
    DS3231_Write_Time();                                // Write time
    DS3231_Read_Time();                                 // Read time
    Initialize_Screen();                                // Initialize the screen before returning  
    In_Time_Setup = 0;
}
    
void Exit_Time_Setup()
{
    DS3231_Read_Time();                                 // Read time
    Initialize_Screen();                                // Initialize the screen before returning  
    In_Time_Setup = 0;
}

void Initialize_Setup_Time_Screen(void) 
{ 
    LCD_Reset();                                    // Screen reset
    TFT_GreenTab_Initialize();         
    fillScreen(ST7735_BLACK);                       // Fills background of screen with color passed to it
    if (Semester == 0)
    {
     Timing_Txt[12] = 'S';  
     Timing_Txt[13] = 'P';         
    }
    else 
    {
     Timing_Txt[12] = 'F';   
     Timing_Txt[13] = 'A';       
    }
    Timing_Txt[14] = Year/10 + '0';
    Timing_Txt[15] = Year%10 + '0';
    Timing_Txt[17] = Session_Number%10 + '0';  
    Timing_Txt[19] = Table_Number/10 + '0';
    Timing_Txt[20] = Table_Number%10 + '0';    
    
    if (SYSTEM_ON == 1)
    {
        drawtext(start_x , start_y, Timing_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);

        strcpy(txt, "Time Setup\0");                                // Text displayed 
        drawtext(start_x+3 , start_y+15, txt, ST7735_MAGENTA, ST7735_BLACK, TS_2); 

        strcpy(txt, "Time");
        drawtext(time_x  , time_y , txt, ST7735_BLUE   , ST7735_BLACK, TS_1);

        fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_CYAN);
        strcpy(txt, "Date");
        drawtext(date_x  , date_y , txt, ST7735_RED    , ST7735_BLACK, TS_1);
    }
}

void Update_Setup_Time_Screen(void)
{
    setup_time[0]  = (setup_hour/10)  + '0';
    setup_time[1]  = (setup_hour%10)  + '0';  
	
    setup_time[3]  = // add code here for minute ten;    
	setup_time[4]  = // add code here for minute one;                  
    
	setup_time[6]  = // add code here for second ten;                       
	setup_time[7]  = // add code here for second one;                       
    
	setup_date[0]  = // add code here for month ten;                       
	setup_date[1]  = // add code here for month one;                        
    
	setup_date[3]  = // add code here for day ten ;                          
	setup_date[4]  = // add code here for day one;                         
    
	setup_date[6]  = // add code here for year ten ;                         
    setup_date[7]  = // add code here for year one;                         

    drawtext(data_time_x, data_time_y, setup_time, ST7735_CYAN, ST7735_BLACK, TS_2);
    drawtext(data_date_x, data_date_y, setup_date, ST7735_GREEN, ST7735_BLACK, TS_2);
}

void Update_Setup_Screen_Cursor_Forward(char field)
{
    char xinc = 36;
    char yinc = 30;   
    switch (field)
    {
        case 0:
            fillRect(data_time_x-1+2*xinc, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+2*xinc, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_CYAN);
            break;
            
        case 1:
            fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+xinc, data_time_y+16, 25,2,ST7735_CYAN);
            break; 
            
        case 2:
            
            fillRect(data_time_x-1+xinc, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+2*xinc, data_time_y+16, 25,2,ST7735_CYAN);
            break;  
            
        case 3:
            fillRect(data_time_x-1+2*xinc, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;   
            
       case 4:
            fillRect(data_time_x-1, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+xinc, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;  

       case 5:
            fillRect(data_time_x-1+xinc, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+2*xinc, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;                           
    }
}

void Update_Setup_Screen_Cursor_Backward(char field)
{
    char xinc = 36;
    char yinc = 30;   
    switch (field)
    {
        case 0:
            fillRect(data_time_x-1+xinc, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_CYAN);
            break;
            
        case 1:
            fillRect(data_time_x-1+2*xinc, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+xinc, data_time_y+16, 25,2,ST7735_CYAN);
            break; 
            
        case 2:
            fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+2*xinc, data_time_y+16, 25,2,ST7735_CYAN);
            break;  
            
        case 3:
            fillRect(data_time_x-1+xinc, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;   
            
       case 4:
            fillRect(data_time_x-1+2*xinc, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+xinc, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;  

       case 5:
            fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_BLACK);           
            fillRect(data_time_x-1+2*xinc, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;              
    }
}




