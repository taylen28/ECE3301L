#include <xc.h>
#include <p18f4620.h>
#include "Interrupt.h"
#include "main.h"

// External variables defined elsewhere in the program
extern char INT0_Flag;
extern char INT1_Flag;
extern char INT2_Flag;
extern char EW_PED_SW;
extern char NS_PED_SW;
extern char Flashing_Request;
extern char MODE;

// ==========================
// INTERRUPT INITIALIZATION
// ==========================
void init_INTERRUPT()
{
    // --- Clear interrupt flags (to avoid false triggers at startup) ---
    INTCONbits.INT0IF   = 0; // Clear external interrupt 0 flag
    INTCON3bits.INT1IF  = 0; // Clear external interrupt 1 flag
    INTCON3bits.INT2IF  = 0; // Clear external interrupt 2 flag
    
    // --- Set interrupt edge detection (0 = falling edge trigger) ---
    INTCON2bits.INTEDG0 = 0; // INT0 triggers on falling edge
    INTCON2bits.INTEDG1 = 0; // INT1 triggers on falling edge
    INTCON2bits.INTEDG2 = 0; // INT2 triggers on falling edge
    
    // --- Enable external interrupts ---
    INTCONbits.INT0IE   = 1; // Enable INT0 interrupt
    INTCON3bits.INT1IE  = 1; // Enable INT1 interrupt
    INTCON3bits.INT2IE  = 1; // Enable INT2 interrupt
    
    // --- Enable global interrupt system ---
    INTCONbits.GIE      = 1; // Global Interrupt Enable (must be set for any interrupt to work)
}

// ==========================
// HIGH PRIORITY ISR HANDLER
// ==========================
void interrupt high_priority chkisr()
{
    // Check which interrupt flag is set and call its corresponding ISR
    
    if (INTCONbits.INT0IF == 1) INT0_ISR(); // INT0 (NS Pedestrian button)
    if (INTCON3bits.INT1IF == 1) INT1_ISR(); // INT1 (EW Pedestrian button)
    if (INTCON3bits.INT2IF == 1) INT2_ISR(); // INT2 (Flashing mode request)
}

// ==========================
// INDIVIDUAL INTERRUPT ISRs
// ==========================

// --- INT0 ISR: Handles the North-South pedestrian button ---
void INT0_ISR()
{
    INTCONbits.INT0IF = 0; // Clear INT0 flag
    if (MODE)             // Only respond if the system is in normal mode
    {
        NS_PED_SW = 1;    // Set flag indicating NS pedestrian button was pressed
    }
}

// --- INT1 ISR: Handles the East-West pedestrian button ---
void INT1_ISR()
{
    INTCON3bits.INT1IF = 0; // Clear INT1 flag
    if (MODE)               // Only respond if system is in normal mode
    {
        EW_PED_SW = 1;      // Set flag indicating EW pedestrian button was pressed
    }
}

// --- INT2 ISR: Handles the Flashing Mode request button ---
void INT2_ISR()
{
    INTCON3bits.INT2IF = 0;  // Clear INT2 flag
    Flashing_Request = 1;    // Set flag to request flashing mode (manual override)
}
