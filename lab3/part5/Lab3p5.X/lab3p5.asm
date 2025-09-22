        #include <P18F4620.inc>

        config OSC = INTIO67
        config WDT = OFF
        config LVP = OFF
        config BOREN = OFF

;---------------- Registers ----------------
Delay1      equ     0xF6
Delay2      equ     0xFF
Counter_L   equ     0x20
Counter_H   equ     0x21
Color_Value equ     0x28
Loop_Count  equ     0x29

        ORG     0x0000
        GOTO    START

;---------------- Main ----------------
START:
        MOVLW   0x0F
        MOVWF   ADCON1          ; make ports digital

        CLRF    TRISC           ; LED1 (D1) on PORTC<2:0>
        CLRF    TRISD           ; LED2 (D2) on PORTD<7:5>
        CLRF    TRISE

; -------- Array 1 (LED1 sequence, 0x40?0x47, PORTC<2:0>) --------
        MOVLW   0x05           ; 0 Purple (101)
        MOVWF   0x40
        MOVLW   0x03           ; 1 Yellow (011)
        MOVWF   0x41
        MOVLW   0x04           ; 2 Blue (100)
        MOVWF   0x42
        MOVLW   0x06           ; 3 Cyan (110)
        MOVWF   0x43
        MOVLW   0x01           ; 4 Red (001)
        MOVWF   0x44
        MOVLW   0x02           ; 5 Green (010)
        MOVWF   0x45
        MOVLW   0x07           ; 6 White (111)
        MOVWF   0x46
        MOVLW   0x00           ; 7 Off (000)
        MOVWF   0x47

; -------- Array 2 (LED2 sequence, 0x48?0x4F, PORTD<7:5>) --------
        MOVLW   0x06<<3           ; 0 Cyan (110 << 5 = 0xC0)
        MOVWF   0x50
        MOVLW   0x07<<3           ; 1 White (111 << 5 = 0xE0)
        MOVWF   0x51
        MOVLW   0x02<<3           ; 2 Green (010 << 5 = 0x40)
        MOVWF   0x52
        MOVLW   0x00<<3           ; 3 Off
        MOVWF   0x53
        MOVLW   0x07<<3           ; 4 White
        MOVWF   0x54
        MOVLW   0x04<<3           ; 5 Blue (100 << 5 = 0x80)
        MOVWF   0x55
        MOVLW   0x05<<3           ; 6 Purple (101 << 5 = 0xA0)
        MOVWF   0x56
        MOVLW   0x01<<3           ; 7 Red (001 << 5 = 0x20)
        MOVWF   0x57

; -------- Loop --------
WHILE_LOOP:
        CLRF    Color_Value
        MOVLW   0x08
        MOVWF   Loop_Count

        LFSR    0, 0x40        ; point FSR0 to Array1
        LFSR    1, 0x50        ; point FSR1 to Array2

FOR_LOOP:
        ; LED1 on PORTC<2:0>
        MOVF    INDF0, W
        MOVWF   PORTC

        ; LED2 on PORTD<7:5>
        MOVF    INDF1, W
        MOVWF   PORTD

        CALL    DELAY_ONE_SEC

        INCF    FSR0L, F
        INCF    FSR1L, F

        DECF    Loop_Count, F
        BNZ     FOR_LOOP

        GOTO    WHILE_LOOP

; -------- Delay Subroutine --------
DELAY_ONE_SEC:
        MOVLW   Delay1
        MOVWF   Counter_H

LOOP_OUTER:
        MOVLW   Delay2
        MOVWF   Counter_L

LOOP_INNER:
        NOP
        DECF    Counter_L, F
        BNZ     LOOP_INNER

        DECF    Counter_H, F
        BNZ     LOOP_OUTER

        RETURN

        END
