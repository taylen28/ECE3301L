
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
				     
    MOVF    PORTA, W		    ; Read the content of PORTA, mask in the lower 4-bit, and store the result into the variable InA
    ANDLW   0x0F		    ; line loaded into the working register data stored in InA
    MOVWF   InA			    ; store the result into the variable InA
				    
    MOVLW   0x09		    ; load the constant 0x09 into W
    CPFSGT  InA, 1		    ; compare the value in InA against the W register. If InA is greater than 0x09, the next instruction below this instruction is executed.
    
    GOTO    LESS		    ; Calls subroutine less when InA is less than 0x09
    GOTO    GREATER		    ; Calls subroutine greater when InA is greater or equal to0x09
    
   
    
GREATER:			    ; subroutine Greater
    MOVLW   0x06		    ; Load a constant 0x06 into the W register
    ADDWF   InA			    ; add a constant 0x06 to InA if it's greater than 0x09
    MOVFF   InA, Result		    ; Copy the result from InA to Result
    MOVFF   Result, PORTC	    ; Display the result out to the LEDs			     
    GOTO    MAIN_LOOP		    ; Go back to the main loop
    
LESS:				    ; Subroutine Less
    MOVFF   InA, Result		    ; Copy the result from InA to Result
    MOVFF   Result, PORTC	    ; Display the result out to the LEDs
    GOTO    MAIN_LOOP		    ; Go back to the main loop
    
 END




