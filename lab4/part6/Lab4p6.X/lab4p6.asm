#include <P18F4620.inc>

    config OSC = INTIO67
    config WDT = OFF
    config LVP = OFF
    config BOREN = OFF

InA    equ 0x20
InB    equ 0x21
Result equ 0x22

    ORG 0x0000

START:
    ; Digital mode
    MOVLW 0x0F
    MOVWF ADCON1

    ; TRIS setup
    MOVLW 0xFF
    MOVWF TRISA      ; PORTA = input
    MOVWF TRISB      ; PORTB = input
    CLRF  TRISC      ; PORTC = output
    CLRF  TRISD      ; PORTD = output
    MOVLW 0x07
    MOVWF TRISE

MAIN_LOOP:
    ; Get 4-bit input from PORTA
    MOVF PORTA, W
    ANDLW 0x0F
    MOVWF InA

    ; Get 4-bit input from PORTB
    MOVF PORTB, W
    ANDLW 0x0F
    MOVWF InB

    ; AND InA & InB
    MOVF InA, W         ; W = InA
    ANDWF InB, W        ; W = InA AND InB

    ; Mask again to keep only 4 bits
    ANDLW 0x0F

    ; Save result
    MOVWF Result

    ; Display on PORTC
    MOVFF Result, PORTC

    GOTO MAIN_LOOP

    END
