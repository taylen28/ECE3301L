#include <P18F4620.inc>

    config OSC = INTIO67
    config WDT = OFF
    config LVP = OFF
    config BOREN = OFF

InA    equ 0x20
Result equ 0x22

    ORG 0x0000

START:
    MOVLW 0x0F
    MOVWF ADCON1

    MOVLW 0xFF
    MOVWF TRISA      ; PORTA = input
    MOVWF TRISB      ; PORTB = input
    CLRF  TRISC      ; PORTC = output
    CLRF  TRISD      ; PORTD = output
    MOVLW 0x07
    MOVWF TRISE      ; RE0?RE2 = inputs

MAIN_LOOP:
    MOVF PORTA, W
    ANDLW 0x0F
    MOVWF InA

    COMF InA, W
    ANDLW 0x0F       ; W = complement (lower nibble), Z updated here

    MOVWF Result
    MOVFF Result, PORTC

    ; Check Z flag directly after ANDLW
    BCF PORTD, 0
    BZ SET_BIT
    
    GOTO TEST_DONE

SET_BIT:
    BSF PORTD, 0

TEST_DONE:
    GOTO MAIN_LOOP

    END
