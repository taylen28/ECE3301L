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






        LIST P=18F4321
        ;========================================
        ; Variable assignments
        ;========================================
A       EQU 0x40
B       EQU 0x41
C       EQU 0x42
X       EQU 0x50

        ORG 0x100            ; Program start

START:
        ;----------------------------
        ; WREG = A * 2
        ;----------------------------
        MOVF    A, W          ; W = A
        ADDWF   A, W          ; W = A + A = 2A
        MOVWF   X             ; X = 2A

        ;----------------------------
        ; X = X + 2B
        ;----------------------------
        MOVF    B, W          ; W = B
        ADDWF   B, W          ; W = 2B
        ADDWF   X, F          ; X = X + 2B

        ;----------------------------
        ; X = X - C
        ;----------------------------
        MOVF    C, W          ; W = C
        SUBWF   X, F          ; X = X - C

        END

[0x10] = 0xF5
[0x20] = 0xFF
[0x30] = 0x27
[0x40] = 0x00
[0x50] = 0x00

        ORG     0x0100  ;
        MOVLW   0x10    ;
        ADDWF   0x10,W  ;
        ADDWFC  0x30,W  ;
        XORWF   0x20,W  ;
        MOVWF   0x40    ;
        INCF    0x20,F  ;
        MOVLW   0XD3    ;
        BZ      LABEL1  ;
        GOTO    LABEL2  ;
    LABEL1              
        MOVWF   0x50    ;
        GOTO    DONE    ;
    LABEL2
        MOVFF   0x40,0x50;   
    DONE    
        SLEEP
        END
                        ;

Assuming that we have an array called A with 10 values. The starting location of
this array is 0x30. We also have another array called B with also 10 
values with the starting location at 0x40. Two variables called
POSCNT and NEGCNT are located at locations 0x70 and 0x71. A
Variable result is located at location 0x80."


write an assembly program to do the following:
    a) at the start clear the values of all the variables POSCNT, NEGCNT and 
    Result to 0 using the CLRF instruction

    b) using a loop move the 10 consecutive values from ARRAy A to Array 
    with the adresses fromm array A increase(LOW TO HIGH) while the ones 
    from array B decreasing (HIGH TO LOW)

    c)While moving one bye of data in part B, check if teh value in each
    location of Array A is positive. If positive increment the variable
    POSCNT. If negative increase the value in NEGCNT.

    d) after all the data are moved(completion of loop) check the value of
    POSCNT against the variable NEGCNT using the CPFSGT or CPFSLT. if
    POSCNT is greater than NEGCNT, Store the value of RESULT with 0x00. if less or equal store
    result with 0xFF. Make sure to use CLRF and SETF instruction to set the value of the variable RESULT