;==================================================
; PIC18F4525 Assembly Program
; Equivalent to provided C code
;==================================================

        LIST    P=18F4525
        INCLUDE <p18f4525.inc>

;--------------------------------------------------
; Configuration Bits
;--------------------------------------------------
        CONFIG  OSC = INTIO67
        CONFIG  WDT = OFF
        CONFIG  LVP = OFF
        CONFIG  BOREN = OFF

;--------------------------------------------------
; Variables
;--------------------------------------------------
        CBLOCK  0x20
        in_sw          ; variable to store switch input
        ENDC

;--------------------------------------------------
; Reset Vector
;--------------------------------------------------
        ORG     0x0000
        GOTO    MAIN

;--------------------------------------------------
; Main Program
;--------------------------------------------------
MAIN:
        ; Setup I/O
        MOVLW   0xFF          ; All inputs on PORTB
        MOVWF   TRISB

        CLRF    TRISC         ; All outputs on PORTC
        CLRF    TRISA         ; All outputs on PORTA
        CLRF    TRISD         ; All outputs on PORTD

        MOVLW   0x0F          ; All digital I/O
        MOVWF   ADCON1

;--------------------------------------------------
; Infinite Loop
;--------------------------------------------------
LOOP:
        MOVF    PORTB, W      ; Read PORTB into WREG
        ANDLW   0x07          ; Mask lower 3 bits (RB0?RB2)
        MOVWF   in_sw         ; Save into variable

        MOVF    in_sw, W      ; Move switch value to WREG
        MOVWF   PORTC         ; Output to PORTC (RC0?RC2 -> RGB LED)

        GOTO    LOOP          ; Repeat forever

        END
