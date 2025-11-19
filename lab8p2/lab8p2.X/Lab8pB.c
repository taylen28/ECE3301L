






#pragma config OSC = INTIO67     // Internal oscillator, I/O on RA6 & RA7
#pragma config WDT = OFF         // Disable Watchdog Timer
#pragma config LVP = OFF         // Disable Low Voltage Programming
#pragma config BOREN = OFF       // Disable Brown-out Reset

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

// === Function Prototypes ===
void Set_NS(char color);
void Set_NSLT(char color);
void Set_EW(char color);
void Set_EWLT(char color);
void PED_Control(char direction, char Num_Sec);
void Day_Mode();
void Night_Mode();
void Wait_N_Seconds(char);
void Wait_One_Second_With_Beep();
void Wait_One_Second();
void Do_Flashing();

// === Global Variables ===
char INT0_Flag;
char INT1_Flag;
char INT2_Flag;

#define _XTAL_FREQ  8000000             // Set system clock to 8 MHz
#define TMR_CLOCK   _XTAL_FREQ/4        // Timer runs at 2 MHz (Fosc/4)

char dir;
char Count;                             // General counter variable
char PED_Count;                         // Pedestrian countdown variable

char MODE;                              // 1 = Day Mode, 0 = Night Mode
char direction;                         // Tracks which direction (NS, EW, etc.)
float volt;                             // Voltage reading from light sensor

extern char Light_Sensor;               // Declared in another file

// Interrupt-related flags (set by ISR routines)
char NS_PED_SW = 0;
char EW_PED_SW = 0;

char Flashing_Request = 0;
char Flashing_Status = 0;

// =====================================================
//                  MAIN FUNCTION
// =====================================================
void main(void)
{
    OSCCON = 0x70;                      // Set internal oscillator to 8 MHz
    TRISA = 0x1F;                       // RA0–RA4 inputs (for sensors), RA5 output
    TRISB = 0x07;                       // RB0–RB2 inputs for external interrupts
    TRISC = 0x00;                       // PORTC as output (LEDs or control signals)
    TRISD = 0x00;                       // PORTD as output
    TRISE = 0x00;                       // PORTE as output

    Init_ADC();                         // Initialize analog-to-digital converter
    Init_UART();                        // Initialize serial UART for debug messages

    Initialize_LCD_Screen();            // Initialize TFT screen for display
    RBPU = 0;                           // Enable PORTB pull-ups (for switches)
    init_INTERRUPT();                   // Enable external interrupts INT0–INT2

    // === LIGHT SENSOR READING ===
    volt = Read_Volt(0);                // Read analog voltage from LDR (RA0)
    Light_Sensor = (volt < 2.5) ? 1 : 0; // Day mode if bright (voltage < 2.5V)

    // === MAIN LOOP ===
    while(1)
    {
        // Select operating mode based on light sensor reading
        if (Light_Sensor == 1)
        {
            Day_Mode();                 // Run traffic cycle for daytime
        }
        else
        {
            Night_Mode();               // Run traffic cycle for nighttime
        }

        // If flashing mode is requested via INT2 interrupt
        if (Flashing_Request == 1)
        {
            Flashing_Request = 0;
            Flashing_Status = 1;
            Do_Flashing();              // Enter flashing mode
        }
    }
}

// =====================================================
//                  FLASHING MODE FUNCTION
// =====================================================
// Flashes all red lights ON and OFF until the user exits flashing mode
void Do_Flashing()
{
    while (Flashing_Status == 1)
    {
        if (Flashing_Request == 0)
        {
            // Turn all lights ON (red)
            Set_NS(RED);
            Set_NSLT(RED);
            Set_EW(RED);
            Set_EWLT(RED);
            Wait_One_Second();

            // Turn all lights OFF
            Set_NS(OFF);
            Set_NSLT(OFF);
            Set_EW(OFF);
            Set_EWLT(OFF);
            Wait_One_Second();
        }
        else if (Flashing_Request == 1)
        {
            // Exit flashing mode when button pressed again
            Flashing_Request = 0;
            Flashing_Status  = 0;
        }
    }
}

// =====================================================
//                TRAFFIC LIGHT CONTROL FUNCTIONS
// =====================================================

// Controls North-South light
void Set_NS(char color)
{
    direction = NS;
    update_LCD_color(direction, color);  // Update LCD indicator

    switch (color)
    {
        case OFF:    NS_RED=0; NS_GREEN=0; break;
        case RED:    NS_RED=1; NS_GREEN=0; break;
        case GREEN:  NS_RED=0; NS_GREEN=1; break;
        case YELLOW: NS_RED=1; NS_GREEN=1; break;
    }
}

// Controls North-South Left Turn light
void Set_NSLT(char color)
{
    direction = NSLT;
    update_LCD_color(direction, color);

    switch (color)
    {
        case OFF:    NSLT_RED=0; NSLT_GREEN=0; break;
        case RED:    NSLT_RED=1; NSLT_GREEN=0; break;
        case GREEN:  NSLT_RED=0; NSLT_GREEN=1; break;
        case YELLOW: NSLT_RED=1; NSLT_GREEN=1; break;
    }
}

// Controls East-West light
void Set_EW(char color)
{
    direction = EW;
    update_LCD_color(direction, color);

    switch (color)
    {
        case OFF:    EW_RED=0; EW_GREEN=0; break;
        case RED:    EW_RED=1; EW_GREEN=0; break;
        case GREEN:  EW_RED=0; EW_GREEN=1; break;
        case YELLOW: EW_RED=1; EW_GREEN=1; break;
    }
}

// Controls East-West Left Turn light
void Set_EWLT(char color)
{
    direction = EWLT;
    update_LCD_color(direction, color);

    switch (color)
    {
        case OFF:    EWLT_RED=0; EWLT_GREEN=0; break;
        case RED:    EWLT_RED=1; EWLT_GREEN=0; break;
        case GREEN:  EWLT_RED=0; EWLT_GREEN=1; break;
        case YELLOW: EWLT_RED=1; EWLT_GREEN=1; break;
    }
}

// =====================================================
//           PEDESTRIAN CONTROL FUNCTION
// =====================================================
// Handles pedestrian countdown for each direction
void PED_Control(char direction, char Num_Sec)
{
    for (int i = Num_Sec; i > 0; i--)
    {
        update_LCD_PED_Count(direction, i - 1);  // Show countdown on LCD
        Wait_One_Second_With_Beep();             // Wait 1 sec with buzzer
    }

    // Reset pedestrian switch flag after countdown completes
    if (direction)
        EW_PED_SW = 0;
    else
        NS_PED_SW = 0;
}

// =====================================================
//                  DAY MODE FUNCTION
// =====================================================
// Full traffic sequence with pedestrian and left-turn logic
void Day_Mode()
{
    MODE     = 1;          // Day mode ON
    MODE_LED = 1;          // Turn on mode LED

    // Initial light state
    Set_EW(RED);
    Set_EWLT(RED);
    Set_NSLT(RED);
    Set_NS(GREEN);

    // NS Pedestrian phase
    if (NS_PED_SW)
        PED_Control(NS, PEDESTRIAN_NS_WAIT);

    Wait_N_Seconds(NS_WAIT);
    Set_NS(YELLOW);
    Wait_N_Seconds(3);
    Set_NS(RED);

    // EW Left Turn phase
    if (EW_LT_SW)
    {
        Set_EWLT(GREEN);
        Wait_N_Seconds(EW_LT_WAIT);
        Set_EWLT(YELLOW);
        Wait_N_Seconds(3);
        Set_EWLT(RED);
    }

    // EW Straight phase
    Set_EW(GREEN);
    if (EW_PED_SW)
        PED_Control(EW, PEDESTRIAN_EW_WAIT);

    Wait_N_Seconds(EW_WAIT);
    Set_EW(YELLOW);
    Wait_N_Seconds(3);
    Set_EW(RED);

    // NS Left Turn phase
    if (NS_LT_SW)
    {
        Set_NSLT(GREEN);
        Wait_N_Seconds(NS_LT_WAIT);
        Set_NSLT(YELLOW);
        Wait_N_Seconds(3);
        Set_NSLT(RED);
    }
}

// =====================================================
//                  NIGHT MODE FUNCTION
// =====================================================
// Simplified cycle with no pedestrian crossings
void Night_Mode()
{
    MODE      = 0;         // Night mode
    MODE_LED  = 0;         // Mode LED OFF
    EW_PED_SW = 0;
    NS_PED_SW = 0;

    Set_EW(RED);
    Set_EWLT(RED);
    Set_NSLT(RED);
    Set_NS(GREEN);

    Wait_N_Seconds(NIGHT_NS_WAIT);
    Set_NS(YELLOW);
    Wait_N_Seconds(3);
    Set_NS(RED);

    if (EW_LT_SW)
    {
        Set_EWLT(GREEN);
        Wait_N_Seconds(NIGHT_EW_LT_WAIT);
        Set_EWLT(YELLOW);
        Wait_N_Seconds(3);
        Set_EWLT(RED);
    }

    Set_EW(GREEN);
    Wait_N_Seconds(NIGHT_EW_WAIT);
    Set_EW(YELLOW);
    Wait_N_Seconds(3);
    Set_EW(RED);

    if (NS_LT_SW)
    {
        Set_NSLT(GREEN);
        Wait_N_Seconds(NIGHT_NS_LT_WAIT);
        Set_NSLT(YELLOW);
        Wait_N_Seconds(3);
        Set_NSLT(RED);
    }
}

// =====================================================
//              DELAY AND TIMER FUNCTIONS
// =====================================================

// Blinks status LED and updates LCD every second
void Wait_One_Second()
{
    SEC_LED = 1;
    Draw_Star();               // Blink '*' on LCD
    Wait_Half_Second();
    SEC_LED = 0;
    Erase_Star();
    Wait_Half_Second();
    update_LCD_misc();         // Refresh time display
}

// Waits one second and beeps the buzzer (for pedestrians)
void Wait_One_Second_With_Beep()
{
    SEC_LED = 1;
    Draw_Star();
    Activate_Buzzer();
    Wait_Half_Second();

    SEC_LED = 0;
    Erase_Star();
    Deactivate_Buzzer();
    Wait_Half_Second();
    update_LCD_misc();
}

// Waits a specified number of seconds and displays countdown
void Wait_N_Seconds(char seconds)
{
    for (char i = seconds; i > 0; i--)
    {
        update_LCD_count(direction, i);
        Wait_One_Second();
    }
    update_LCD_count(direction, 0);
}
