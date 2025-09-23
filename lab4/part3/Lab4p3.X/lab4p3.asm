#include<P18F4620.inc>
    config OSC = INTIO67
    config WDT = OFF
    config LVP = OFF
    config BOREN = OFF
; Constant declarations
InA	equ		0x20
InB	equ		0x21
Result	equ		0x22
    ORG		0x0000		    ; orgin of the program memory and where it is stored
 ; CODE STARTS FROM THE NEXT LINE
START:				    ; start the main proagram
    MOVLW   0x0F		    ; line loaded into the working register data stored in ADCON1
    MOVWF   ADCON1		    ; working register data stored in ADCON1
    MOVLW   0xFF		    ; TRISA, TRISB is set to be an intput
    MOVWF   TRISA		    ; TRISA is an input		    
    MOVWF   TRISB		    ; TRISB is an input
    MOVLW   0x07		    ; Set to be an output and input
    MOVWF   TRISD		    ; TRISD is an input and output
				    
    MOVLW   0x00		    ; TRISC and TRISE is set to be an output
    MOVWF   TRISC		    ; Is an output
    MOVWF   TRISE		    ; Is an output
MAIN_LOOP:			    ; loop that will repeat indefinitely				     
    MOVF PORTA, W		    ; Read the content of PORTA, mask in the lower 4-bit 
    ANDLW 0x0F			    ; line loaded into the working register data stored in InA
    MOVWF InA			    ; store the result into the variable InA
				    
    MOVF    PORTB, W		    ; Read the content of PORTB, mask in the lower 4-bit, and store the result into the variable InB
    ANDLW   0x0F		    ; line loaded into the working register data stored in InA
    ADDWF   InA, W		    ; Add InB to InA and store the result into the variable Result
    MOVWF   Result		    ; Result stored from InA plus InB
				    
    MOVFF   Result, PORTC	    ; Display the result out to the LEDs
				     
				    ; Check if the Zero (Z flag) is set
    BZ      SET_PORTE_BIT0	    ; If Z flag is 1, force a jump to SET_PORTE_BIT0 label
				    ; If Z flag is 0, execute the instruction just below BZ instruction
				    ; and clear PORTE bit 0 to 0
    BCF     PORTE, 0		    ; Clear PORTE bit 0 to be 0
    GOTO    MAIN_LOOP		    ; Go back to the main loop
SET_PORTE_BIT0:			    ; Subroutine to set PORTE from 0 to 1
    BSF     PORTE, 0		    ; Set PORTE bit 0 to 1   
    GOTO    MAIN_LOOP		    ; jumps back to the beginning of the MAIN_LOOP label, creating an infinite loop.
 END



