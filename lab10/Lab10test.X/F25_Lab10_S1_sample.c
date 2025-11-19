#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>

#pragma config OSC = INTIO67   // Use internal oscillator, I/O on RA6 & RA7
#pragma config WDT = OFF       // Disable Watchdog Timer
#pragma config LVP = OFF       // Disable Low Voltage Programming
#pragma config BOREN = OFF     // Disable Brown-out Reset
//#pragma config CCP2MX = PORTBE // Optional: maps CCP2 to RE7 if needed

#include "ST7735_TFT.h"        // LCD screen control header
#include "utils.h"             // Contains helper functions like delay, buzzer, etc.
#include "Main_Screen.h"       // LCD display layout
#include "Interrupt.h"         // NEC infrared interrupt setup and handlers

#define _XTAL_FREQ  8000000    // Define system clock frequency as 8 MHz


// ------------------- GLOBAL VARIABLES -------------------
short Nec_OK = 0;              // Flag that indicates an IR signal has been fully received
char Nec_Button;               // Stores decoded button value from the IR remote
extern unsigned long long Nec_code; // Stores the full 64-bit NEC IR code

// ------------------- ENUM DEFINITIONS -------------------
typedef enum {
    OFF, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE
} colors;                      // Used for readability when referring to colors

// ------------------- REMOTE BUTTON TABLES -------------------
char array1[21] = {
    0xA2, 0x62, 0xE2, 0x22, 0x02, 0xC2, 0xE0, 0xA8, 0x90,
    0x68, 0x98, 0xB0, 0x30, 0x18, 0x7A, 0x10, 0x38, 0x5A, 0x42, 0x4A, 0x52
}; // NEC button codes

char txt1[21][4] = {
    "CH-\0", " CH\0", "CH+\0", "PRV\0", "NXT\0", "PAU\0", "VL-\0", "VL+\0", " EQ\0",
    " 0 \0", "100\0", "200\0", " 1 \0", " 2 \0", " 3 \0", " 4 \0", " 5 \0", " 6 \0", " 7 \0", " 8 \0", " 9 \0"
}; // Labels for displaying on LCD screen

// Colors for drawing on LCD screen
int color[21] = {
    RD, RD, RD, CY, CY, GR, BU, BU, MA,
    BK, BK, BK, BK, BK, BK, BK, BK, BK, BK, BK, BK
};

// RGB LED color mapping (each corresponds to the button pressed)
int color2[21] = {
    RED, RED, RED, CYAN, CYAN, GREEN, BLUE, BLUE, MAGENTA,
    WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE
};

// Determines which LED indicator (D1/D2/D3) activates for each button
int nums[21] = {
    0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1
};

// ------------------- RGB LED PORT POINTERS -------------------
extern volatile unsigned char* RGB_LED[] = { &PORTB, &PORTA, &PORTA };
// These point to the PORT registers for each LED group (D1 → PORTB, D2/D3 → PORTA)

// Bit position offsets for each LED group
char off[] = {3, 0, 3};


// ============================================================
//                       MAIN FUNCTION
// ============================================================
void main() {
    // ---------- INITIALIZATION ----------
    Init_UART();               // Setup UART for serial debugging (printf output)
    OSCCON = 0x70;             // Set internal oscillator to 8 MHz
    nRBPU = 0;                 // Enable PORTB internal pull-up resistors

    // Set up data directions for I/O ports
    TRISB = 0x01;              // RB0 as input (for IR receiver), rest outputs
    TRISC = 0x00;              // PORTC all outputs
    TRISD = 0x00;              // PORTD all outputs (LEDs, buzzer)
    TRISA = 0x00;              // PORTA all outputs (RGB LEDs)
    TRISE = 0x00;              // PORTE all outputs

    ADCON1 = 0x0F;             // Configure all analog pins as digital I/O

    // Clear all port output values initially
    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;

    // Initialize LCD display and IR interrupt system
    Initialize_LCD_Screen();
    Init_Interrupt();

    Nec_code = 0x0;            // Clear any previous NEC data

    // ---------- MAIN LOOP ----------
    while (1) {
        // Check if a full NEC IR code has been received
        if (Nec_OK == 1) {
            Nec_OK = 0;                    // Reset flag
            Enable_INT_Interrupt();        // Re-enable IR interrupt for next signal
            printf("NEC_Button = %x \r\n", Nec_Button); // Print received code

            char found = 0xff;             // Initialize "not found" flag

            // Loop through array to find which button was pressed
            for (int i = 0; i < 21; i++) {
                if (array1[i] == Nec_Button) {
                    found = i;
                }
            }

            // If valid button found
            if (found != 0xff) {
                printf("Key Location = %d \r\n\n", found);

                // ---------- LCD DISPLAY UPDATE ----------
                // Draw a filled colored circle and display the button label
                fillCircle(Circle_X, Circle_Y, Circle_Size, color[found]);
                drawCircle(Circle_X, Circle_Y, Circle_Size, ST7735_WHITE);
                drawtext(Text_X, Text_Y, txt1[found], ST7735_WHITE, ST7735_BLACK, TS_1);

                // ---------- RGB LED OUTPUT ----------
                // Turn off all LEDs first
                PORTA = 0;
                PORTB = 0;
                PORTCbits.RC0 = nums[found];  // Set indicator bit on PORTC (D1/D2/D3)

                // Determine which RGB LED group to light up
                char LED_Sel;
                if (found >= 0 && found <= 2) {
                    LED_Sel = 0;  // D1 group
                }
                else if (found >= 3 && found <= 8) {
                    LED_Sel = 1;  // D2 group
                }
                else {
                    LED_Sel = 2;  // D3 group
                }

                // Apply color value to selected RGB LED group using bit masking
                *RGB_LED[LED_Sel] =
                    (*RGB_LED[LED_Sel] & (~(0b00000111 << off[LED_Sel]))) |
                    ((color2[found] & 0b00000111) << off[LED_Sel]);

                // ---------- BUZZER & KEY PRESSED INDICATOR ----------
                // Turn on LED and buzzer to confirm a button press
                PORTDbits.RD7 = 1;      // Turn on KEY_PRESSED LED
                Activate_Buzzer();      // Play buzzer sound
                Wait_One_Sec();         // Keep it active for 1 second
                Deactivate_Buzzer();    // Stop buzzer
                PORTDbits.RD7 = 0;      // Turn off KEY_PRESSED LED
            }
        }
    }
}
