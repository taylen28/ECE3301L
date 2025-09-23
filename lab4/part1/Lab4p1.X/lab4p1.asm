#include<P18F4620.inc>
    config OSC = INTIO67
    config WDT = OFF
    config LVP = OFF
    config BOREN = OFF
; Constant declarations
InA	equ		0x20
InB	equ		0x21
Result	equ		0x22
    ORG		0x0000			    ; orgin of the program memory and where it is stored
 ; CODE STARTS FROM THE NEXT LINE
START:					    ; start the main proagram
    MOVLW   0x0F			    ; line loaded into the working register data stored in ADCON1
    MOVWF   ADCON1			    ; working register data stored in ADCON1
    MOVLW   0xFF			    ; TRISA, TRISB and is set to be an intput
    MOVWF   TRISA			    ; TRISA is an input		    
    MOVWF   TRISB			    ; TRISAB is an input
    MOVLW   0x07			    ; set to be an output
    MOVWF   TRISD			    ; TRISD is an output and input
					    
    MOVLW   0x00			    ; TRISC and TRISE is set to be an output
    MOVWF   TRISC			    ; is an  output
    MOVWF   TRISE			    ; is an output
MAIN_LOOP:				    ; loop that will repeat indefinitely
				     
    MOVF    PORTA, W			    ; Read the content of PORTA, mask in the lower 4-bit, 
    ANDLW   0x0F			    ; line loaded into the working register data stored in InA
    MOVWF   InA				    ; store the result into the variable InA
					     
    COMF    InA, W			    ; Take the 1's complement of InA and store it into Result
    MOVWF   Result			    ; Result stored from InA 
					    
    MOVFF   Result, PORTC		    ; Display the result out to the LEDs
    GOTO    MAIN_LOOP			    ; jumps back to the beginning of the MAIN_LOOP label, creating an infinite loop.

 END






