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
    MOVLW   0xFF		    ; TRISA, TRISB and is set to be an intput
    MOVWF   TRISA		    ; TRISA is an input		    
    MOVWF   TRISB		    ; TRISAB is an input
    MOVLW   0x07		    ; set to be an output
    MOVWF   TRISD		    ; TRISD is an output and input
					    
    MOVLW   0x00		    ; TRISC and TRISE is set to be an output
    MOVWF   TRISC		    ; is an  output
    MOVWF   TRISE		    ; is an output
    
MAIN_LOOP:
    BTFSC   PORTD, 2		    ; checks if bit 2 is 0 or 1 
    GOTO    PORTD2EQ1		    ; If it's 1, go to task BCD
    GOTO    PORTD2EQ0		    ; If it's 0, check bit 1 of PORTD2EQU0
PORTD2EQ1:
    GOTO    TASK_BCD		    ; goes to TASK_BCD
PORTD2EQ0:
    BTFSC   PORTD, 1		    ; checks if bit 1 is 0 or 1 
    GOTO    PORTD21EQ01		    ; If it's 1, go to PORTD21EQ01
    GOTO    PORTD21EQ00		    ; If it's 0,go to PORTD21EQ00
PORTD21EQ01:
    BTFSC   PORTD, 0		    ; checks if bit 1 is 0 or 1 
    GOTO    PORTD22EQ011	    ; If it's 1, go to PORTD22EQ011
    GOTO    PORTD22EQ010	    ; If it's 0,go to PORTD22EQ010
PORTD22EQ011:
    GOTO    TASK_XOR		    ; goes to TASK_XOR
PORTD22EQ010:
    GOTO    TASK_AND		    ; goes to TASK_AND
PORTD21EQ00:
    BTFSC   PORTD, 0		    ; checks if bit 0 is 0 or 1 
    GOTO    PORTD23EQ001	    ; If it's 1,go to PORTD23EQ001
    GOTO    PORTD23EQ000	    ; If it's 0,go to PORTD23EQ000
PORTD23EQ001:
    GOTO    TASK_ADD		    ; goes to TASK_ADD
PORTD23EQ000:
    GOTO    TASK_COMP		    ; goes to TASK_COMP
    
TASK_COMP:
    BCF	    PORTD, 7		    ; This is to clear the Blue LED of the RGB
    BCF	    PORTD, 6		    ; This is to clear the Green LED of the RGB
    BCF	    PORTD, 5		    ; This is to clear the LED LED of the RGB
    CALL    SUB_COMP		    ; calls subroutine COMP
    GOTO    MAIN_LOOP		    ; Jumps back to main loop
TASK_ADD:
    BCF	    PORTD, 7		    ; This is to clear the LED LED of the RGB
    BCF	    PORTD, 6		    ; This is to clear the LED LED of the RGB
    BSF	    PORTD, 5		    ; sets the most significant bit to 1
    CALL    SUB_ADD		    ; calls subroutine ADD
    GOTO    MAIN_LOOP		    ; Jumps back to main loop
TASK_AND:
    BCF	    PORTD, 7		    ; This is to clear the Blue LED of the RGB
    BSF	    PORTD, 6		    ; sets the most significant bit to 1
    BCF	    PORTD, 5		    ; This is to clear the LED LED of the RGB
    CALL    SUB_AND		    ; calls subroutine AND
    GOTO    MAIN_LOOP		    ; Jumps back to main loop
TASK_XOR:
    BCF	    PORTD, 7		    ; This is to clear the Blue LED of the RGB
    BSF	    PORTD, 6		    ; sets the most significant bit to 1
    BSF	    PORTD, 5		    ; sets the most significant bit to 1
    CALL    SUB_XOR		    ; calls subroutine XOR
    GOTO    MAIN_LOOP		    ; Jumps back to main loop
TASK_BCD:
    BSF	    PORTD, 7		    ; sets the most significant bit to 1
    BCF	    PORTD, 6		    ; This is to clear the Green LED of the RGB
    BCF	    PORTD, 5		    ; This is to clear the LED LED of the RGB
    CALL    SUB_BCD		    ; calls subroutine BCD
    GOTO    MAIN_LOOP		    ; Jumps back to main loop

SUB_COMP:
    MOVF    PORTA, W		    ; Read the content of PORTA, mask in the lower 4-bit, and store the result into the variable InA
    ANDLW   0x0F		    ; line loaded into the working register data stored in InA
    MOVWF   InA			    ; store the result into the variable InA
				     
    COMF    InA, W		    ; Take the 1's complement of InA and store it into Result
    ANDLW   0x0F		    ; line loaded into the working register data stored in InA
    MOVWF   Result		    ; Result stored from InA
				     
    MOVFF   Result, PORTC	    ; Display the result out to the LEDs
				    ; Check if the Zero (Z flag) is set
    BZ      SET_PORTE_BIT0	    ; If Z flag is 1, force a jump to SET_PORTE_BIT0 label
				    ; If Z flag is 0, execute the instruction just below BZ instruction
				    ; and clear PORTE bit 0 to 0
    BCF     PORTE, 0		    ; Clear PORTE bit 0 to be 0
    RETURN			    ; Go back to the main loop
SET_PORTE_BIT0:
    BSF     PORTE, 0		    ; Set PORTE bit 0 to 1   
    RETURN			    ; jumps back to the beginning of the MAIN_LOOP label, creating an infinite loop.
 
    
SUB_ADD:
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
    RETURN			    ; Go back to the main loop
    
SUB_AND:
    MOVF    PORTA, W		    ; Read the content of PORTA, mask in the lower 4-bit, and store the result into the variable InA
    ANDLW   0x0F		    ; line loaded into the working register data stored in InA
    MOVWF   InA			    ; store the result into the variable InA
				    
    MOVF    PORTB, W		    ; Read the content of PORTB, mask in the lower 4-bit, and store the result into the variable InB
    ANDLW   0x0F		    ; line loaded into the working register data stored in InA
    ANDWF   InA, W		    ; AND InB with InA and store the result into the W register
    MOVWF   Result		    ; result from InB with InA
				    	    
    MOVFF   Result, PORTC	    ; Display the result out to the LEDs
				    ; 
				    ; Check if the Zero (Z flag) is set
    BZ      SET_PORTE_BIT0	    ; If Z flag is 1, force a jump to SET_PORTE_BIT0 label
				    ; If Z flag is 0, execute the instruction just below BZ instruction
				    ; and clear PORTE bit 0 to 0
    BCF     PORTE, 0		    ; Clear PORTE bit 0 to be 0
    RETURN			    ; Go back to the main loop
 
    
SUB_XOR:
    MOVF    PORTA, W		    ; Read the content of PORTA, mask in the lower 4-bit, and store the result into the variable InA
    ANDLW   0x0F		    ; line loaded into the working register data stored in InA
    MOVWF   InA			    ; store the result into the variable InA
				    
    MOVF    PORTB, W		    ; Read the content of PORTB, mask in the lower 4-bit, and store the result into the variable InB
    ANDLW   0x0F		    ; line loaded into the working register data stored in InA
    XORWF   InA, W		    ; XOR InB with InA and store the result into the variable Result
    MOVWF   Result		    ; result into the variable Result
    MOVFF   Result, PORTC	    ; Display the result out to the LEDs
				    ; 
				    ; Check if the Zero (Z flag) is set
    BZ      SET_PORTE_BIT0	    ; If Z flag is 1, force a jump to SET_PORTE_BIT0 label
				    ; If Z flag is 0, execute the instruction just below BZ instruction
				    ; and clear PORTE bit 0 to 0
    BCF     PORTE, 0		    ; Clear PORTE bit 0 to be 0
    RETURN			    ; Go back to the main loop
 
    
SUB_BCD:
    MOVF    PORTA, W		    ; Read the content of PORTA, mask in the lower 4-bit, and store the result into the variable InA
    ANDLW   0x0F		    ; line loaded into the working register data stored in InA
    MOVWF   InA			    ; store the result into the variable InA
				    
    MOVLW   0x09		    ; load the constant 0x09 into W
    CPFSGT  InA, 1		    ; compare the value in InA against the W register. If InA is greater than 0x09, the next instruction below this instruction is executed.
    
    GOTO    LESS		    ; Calls subroutine less when InA is less than 0x09
    GOTO    GREATER		    ; Calls subroutine greater when InA is greater or equal to0x09
    
GREATER:			    ;
    MOVLW   0x06		    ; Load a constant 0x06 into the W register
    ADDWF   InA			    ; add a constant 0x06 to InA if it's greater than 0x09
    MOVFF   InA, Result		    ; Copy the result from InA to Result
    MOVFF   Result, PORTC	    ; Display the result out to the LEDs
				    ; Check if the Zero (Z flag) is set
    BZ      SET_PORTE_BIT0	    ; If Z flag is 1, force a jump to SET_PORTE_BIT0 label
				    ; If Z flag is 0, execute the instruction just below BZ instruction
				    ; and clear PORTE bit 0 to 0
    BCF     PORTE, 0		    ; Clear PORTE bit 0 to be 0
    RETURN			    ; goes back to main loop
LESS:				    ;
    MOVFF   InA, Result		    ; Copy the result from InA to Result
    MOVFF   Result, PORTC	    ; Display the result out to the LEDs
    				    ; Check if the Zero (Z flag) is set
    BZ      SET_PORTE_BIT0	    ; If Z flag is 1, force a jump to SET_PORTE_BIT0 label
				    ; If Z flag is 0, execute the instruction just below BZ instruction
				    ; and clear PORTE bit 0 to 0
    BCF     PORTE, 0		    ; Clear PORTE bit 0 to be 0
    RETURN			    ; goes back to main loop
    
END
 




