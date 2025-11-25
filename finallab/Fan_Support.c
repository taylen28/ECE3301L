#include <p18f4620.h>
#include "Main.h"
#include "Fan_Support.h"
#include "stdio.h"
#include "utils.h"

extern char FAN;
extern char FANMODE;
extern char duty_cycle;
extern char fan_set_temp;
extern signed char tempF;
extern char FAN_TIMER_MODE, FAN_TIMER_TIME;

char Timer_Table[4]={Timer0,Timer1,Timer2,Timer3};

int get_duty_cycle(int temp, int set_temp)
{	
    int dc;
// add code here to compare the two variables 'temp' and 'set_temp':
// if 'temp' is less than 'set_temp', then set the variable 'dc' to zero and return that value of 'dc'
// if 'temp' is greater than the 'set_temp', then the value of the variable 'dc' is equal to 3 times the difference between the two temperatuees
// if the calculated value of 'dc' is greater than 100, set it to 100. Return that value of 'dc'

    return (dc);
}

void Monitor_Fan()
{
// add code to do the following:
// Task #1: check if FANMODE is 1
//   if yes, then the value of the variable 'duty_cycle' is equal to the value returned by the function get_duty_cycle (tempF, fan_set_temp);
//          Next, call the function do_update_pwm(duty_cycle);
//   if not, do nothing

// Task #2: check if FAN_TIMER_MODE is not zero.
// 	 If not equal to 0, then check if FAN_TIMER_TIME is not zero.
//       if FAN_TIMER_TIME is not zero, 
//			decrement it by 1
//          check is the new value of FAN_TIMER_TIME is 0. 
//             If so, set the FAN_TIMER_MODE to be 0
//                    call the routine Turn_Off_Fan()
// 


int get_RPM()
{
// copy this routine from lab 12
}

void Toggle_Fan()
{
// this was implemented on lab 12. If variable FAN is 1, call routine to shut off fan. If variable is 0, 
// then call routine to turn on fan
}

void Turn_Off_Fan()
{
    printf ("Fan is turned off\r\n");
//  Set FAN to 1 as well as FAN_EN and FANON_LED to 1
}

void Turn_On_Fan()
{
    printf ("Fan is turned on\r\n");
//  Set FAN to 0 as well as FAN_EN and FANON_LED to 0
    do_update_pwm(duty_cycle);
}


void Increase_Duty_Cycle()
{
    if (FANMODE == 0)
    {    
// check if duty_cycle is already at 100. If so, call DO_BEEP() twice and call do_update_pwm(duty_cycle)
// if not at 100, then add 5 to duty_cycle and also call do_update_pwm(duty_cycle)
    }
}

void Decrease_Duty_Cycle()
{
// check if duty_cycle is already at 0. If so, call DO_BEEP() twice and call do_update_pwm(duty_cycle)
// if not at 0, then subtract 5 to duty_cycle and also call do_update_pwm(duty_cycle)
    }
        }
    }
}

void Do_Change_Fan_Timer_Mode_Plus()
{
    if (FAN_TIMER_MODE < FAN_TIMER_MODE3)
    {
        FAN_TIMER_MODE++;
        FAN_TIMER_TIME= Timer_Table[FAN_TIMER_MODE];
        Do_Beep_Good(); 
    }
    else Do_Beep_Bad();

}

void Do_Change_Fan_Timer_Mode_Minus()
{
    if (FAN_TIMER_MODE  == FAN_TIMER_MODE0)
    {
        Do_Beep_Bad();
    }
    else
    {
        FAN_TIMER_MODE--;
        FAN_TIMER_TIME= Timer_Table[FAN_TIMER_MODE];
        Do_Beep_Good(); 
    }

}




