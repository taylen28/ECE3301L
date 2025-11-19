#include <stdint.h> 
#include <xc.h>
#include <p18f4620.h>
#include "Interrupt.h"

// ---------------- GLOBAL VARIABLES ----------------
unsigned char bit_count;            // Keeps track of how many bits have been received in the NEC code
unsigned int Time_Elapsed;          // Stores the pulse width measured using Timer1

unsigned long long Nec_code;        // Holds the 32-bit NEC data packet
unsigned char Nec_State = 0;        // Tracks the current state of NEC decoding state machine

extern char Nec_Button;             // Decoded button value (from main.c)
extern short Nec_OK;                // Flag set when a full NEC code has been received

uint8_t portE_mask = 0b00000111;    // Mask for displaying current state (3 LSBs of PORTE LEDs)


// ============================================================
//  Function: Init_Interrupt()
//  Purpose:  Initializes INT0 external interrupt and Timer1 for IR decoding
// ============================================================
void Init_Interrupt()
{
    INTCONbits.INT0IF = 0;          // Clear external interrupt flag for INT0
    INTCON2bits.INTEDG0 = 0;        // Configure INT0 to trigger on falling edge (H→L)
    INTCONbits.INT0IE = 1;          // Enable external interrupt INT0

    TMR1H = 0;                      // Clear Timer1 high byte
    TMR1L = 0;                      // Clear Timer1 low byte
    PIR1bits.TMR1IF = 0;            // Clear Timer1 interrupt flag
    PIE1bits.TMR1IE = 1;            // Enable Timer1 interrupt
    INTCONbits.PEIE = 1;            // Enable peripheral interrupts
    INTCONbits.GIE = 1;             // Enable global interrupts
}


// ============================================================
//  Function: Enable_INT_Interrupt()
//  Purpose:  Re-enables INT0 external interrupt for next IR signal
// ============================================================
void Enable_INT_Interrupt()
{
    INTCONbits.INT0IE = 1;          // Enable external interrupt 0
    INTCON2bits.INTEDG0 = 0;        // Trigger on falling edge
}    


// ============================================================
//  Function: chkisr()
//  Purpose:  High-priority interrupt vector — calls correct ISR handler
// ============================================================
void interrupt high_priority chkisr()
{
    if (PIR1bits.TMR1IF == 1) TIMER1_isr();  // If Timer1 overflow, call its ISR
    if (INTCONbits.INT0IF == 1) INTx_isr();  // If external interrupt (IR input), call INTx ISR
}


// ============================================================
//  Function: TIMER1_isr()
//  Purpose:  Handles Timer1 overflow → resets decoding if signal too long
// ============================================================
void TIMER1_isr(void)
{
    Nec_State = 0;                  // Reset decoding process
    INTCON2bits.INTEDG0 = 0;        // Set INT0 to detect falling edge
    T1CONbits.TMR1ON = 0;           // Stop Timer1
    PIR1bits.TMR1IF = 0;            // Clear Timer1 interrupt flag
}


// ============================================================
//  Function: Reset_Nec_State()
//  Purpose:  Helper function to reset decoding state and disable Timer1
// ============================================================
void Reset_Nec_State()
{
    Nec_State = 0;                  // Reset NEC decoding state
    T1CONbits.TMR1ON = 0;           // Turn off Timer1
}


// ============================================================
//  Function: INTx_isr()
//  Purpose:  Handles external interrupt from IR receiver (INT0 pin)
//  Notes:    Uses Timer1 to measure pulse widths and decode NEC protocol
// ============================================================
void INTx_isr(void)
{
    INTCONbits.INT0IF = 0;          // Clear external interrupt flag (INT0)

    // If not the first state, record pulse width using Timer1
    if (Nec_State != 0)
    {
        Time_Elapsed = (TMR1H << 8) | TMR1L;   // Combine high + low bytes
        TMR1H = 0;                             // Reset Timer1
        TMR1L = 0;
    }
    
    switch(Nec_State)
    {
        // ---------------- STATE 0 ----------------
        // Wait for initial falling edge (start of signal)
        case 0 :
        {
            // Clear and start Timer1 to begin timing pulses
            TMR1H = 0;
            TMR1L = 0;
            PIR1bits.TMR1IF = 0;
            T1CON = 0x90;               // Timer1: 1µs per tick @ 8MHz
            T1CONbits.TMR1ON = 1;       // Enable Timer1
            bit_count = 0;              // Reset bit counter
            Nec_code = 0;               // Reset NEC code storage
            Nec_State = 1;              // Move to next state

            // Display current state on LEDs (for debugging)
            PORTE = (PORTE & (~portE_mask)) | (Nec_State & portE_mask);

            INTCON2bits.INTEDG0 = 1;    // Change to detect rising edge next
            return;
        }
        
        // ---------------- STATE 1 ----------------
        // Detect 9ms leading pulse (start signal)
        case 1 :
        {
            if (Time_Elapsed >= 8500 && Time_Elapsed <= 9500) {
                Nec_State = 2;           // Detected valid 9ms pulse
                PORTE = (PORTE & (~portE_mask)) | (Nec_State & portE_mask);
            } 
            else {
                Reset_Nec_State();       // Invalid pulse → reset
            }
            
            INTCON2bits.INTEDG0 = 0;    // Next edge: falling
            return;
        }
        
        // ---------------- STATE 2 ----------------
        // Detect 4.5ms space following 9ms header
        case 2 :
        {
            if (Time_Elapsed >= 4000 && Time_Elapsed <= 5000) {
                Nec_State = 3;           // Move to bit capture state
                PORTE = (PORTE & (~portE_mask)) | (Nec_State & portE_mask);
            } 
            else {
                Reset_Nec_State();
            }
            
            INTCON2bits.INTEDG0 = 1;    // Next edge: rising
            return;
        }
        
        // ---------------- STATE 3 ----------------
        // Wait for next falling edge (start of bit pulse)
        case 3 :
        {
            if (Time_Elapsed >= 400 && Time_Elapsed <= 700) {
                Nec_State = 4;           // Valid start of bit detected
                PORTE = (PORTE & (~portE_mask)) | (Nec_State & portE_mask);
            } 
            else {
                Reset_Nec_State();
            }
            
            INTCON2bits.INTEDG0 = 0;    // Next edge: falling
            return;
        }
        
        // ---------------- STATE 4 ----------------
        // Measure pulse width to determine bit value (0 or 1)
        case 4 :
        {
            if (Time_Elapsed >= 400 && Time_Elapsed <= 1800) {
                
                // Shift left to make room for next bit
                Nec_code = Nec_code << 1;

                // NEC logic:
                // ~560µs pulse = '0'
                // ~1690µs pulse = '1'
                if (Time_Elapsed > 1000)
                    Nec_code++;          // If long pulse, record '1'

                bit_count++;             // Count the bit

                if (bit_count > 31) {    // 32 bits complete
                    Nec_Button = Nec_code >> 8;   // Extract command byte
                    Nec_State = 0;       // Reset state machine
                    PORTE = (PORTE & (~portE_mask)) | (Nec_State & portE_mask);
                    Nec_OK = 1;          // Signal main program: full code received
                    INT0IE = 0;          // Disable further IR interrupts until handled
                }
                else {
                    // Not done yet → go back to waiting for next bit
                    Nec_State = 3;
                    PORTE = (PORTE & (~portE_mask)) | (Nec_State & portE_mask);
                }
            } 
            else {
                Reset_Nec_State();       // Invalid pulse → reset
            }
            
            INTCON2bits.INTEDG0 = 1;    // Next edge: rising
            return;
        }
    }
}
