#include <P18F4620.inc>

    config OSC  = INTIO67
    config WDT  = OFF
    config LVP  = OFF
    config BOREN= OFF

; ---------------- Vars ----------------
InA     equ 0x20
InB     equ 0x21
Result  equ 0x22

    ORG 0x0000

START:
    ; All digital
    MOVLW   0x0F
    MOVWF   ADCON1

    ; Directions
    MOVLW   0xFF
    MOVWF   TRISA          ; PORTA inputs (DIP)
    MOVWF   TRISB          ; PORTB inputs (DIP)
    CLRF    TRISC          ; PORTC outputs (8 LEDs)
    CLRF    TRISD          ; PORTD outputs (RGB on D7..D5, Z on D0)
    MOVLW   0x07
    MOVWF   TRISE          ; RE2..RE0 inputs (function select)

; ------------- Main decoder -------------
MAIN_LOOP:
    BTFSC   PORTE, 2
    GOTO    PE2_EQ1
    GOTO    PE2_EQ0

PE2_EQ1:
    BTFSC   PORTE, 1
    GOTO    TASK_BCD       ; 11x
    GOTO    TASK_AND       ; 10x

PE2_EQ0:
    BTFSC   PORTE, 1
    GOTO    PE21_EQ01      ; 01x
    GOTO    PE21_EQ00      ; 00x

PE21_EQ01:
    BTFSC   PORTE, 0
    GOTO    TASK_OR        ; 011
    GOTO    TASK_XOR       ; 010

PE21_EQ00:
    BTFSC   PORTE, 0
    GOTO    TASK_ADD       ; 001
    GOTO    TASK_COMP      ; 000

; ------------- Task wrappers (RGB on D7..D5) -------------
TASK_COMP:
    BCF PORTD,7
    BCF PORTD,6
    BCF PORTD,5            ; 000
    CALL SUB_COMP
    GOTO MAIN_LOOP

TASK_ADD:
    BCF PORTD,7
    BCF PORTD,6
    BSF PORTD,5            ; 001
    CALL SUB_ADD
    GOTO MAIN_LOOP

TASK_XOR:
    BCF PORTD,7
    BSF PORTD,6
    BCF PORTD,5            ; 010
    CALL SUB_XOR
    GOTO MAIN_LOOP

TASK_OR:
    BCF PORTD,7
    BSF PORTD,6
    BSF PORTD,5            ; 011
    CALL SUB_OR
    GOTO MAIN_LOOP

TASK_AND:
    BSF PORTD,7
    BCF PORTD,6
    BCF PORTD,5            ; 100
    CALL SUB_AND
    GOTO MAIN_LOOP

TASK_BCD:
    BSF PORTD,7
    BCF PORTD,6
    BSF PORTD,5            ; 110
    CALL SUB_BCD
    GOTO MAIN_LOOP

; ------------- Subroutines -------------
; Each sub shows result on PORTC and sets Z LED on PORTD.0 if result==0
; Use Z directly from last ALU op (no MOVF needed).

; -- 1) 1?s Complement of PORTA[3:0] --
SUB_COMP:
    MOVF    PORTA, W
    ANDLW   0x0F
    MOVWF   InA
    COMF    InA, W
    ANDLW   0x0F                ; Z updated here for zero test
    MOVWF   Result
    MOVFF   Result, PORTC

    ; Z LED (D0)
    BCF     PORTD, 0
    BZ      ZSET_COMP
    RETURN
ZSET_COMP:
    BSF     PORTD, 0
    RETURN

; -- 2) ADD (PORTA + PORTB) & 0x0F --
SUB_ADD:
    MOVF    PORTA, W
    ANDLW   0x0F
    MOVWF   InA
    MOVF    PORTB, W
    ANDLW   0x0F                ; W = InB
    ADDWF   InA, W              ; W = InA + InB
    ANDLW   0x0F                ; keep 4 bits, Z from ANDLW
    MOVWF   Result
    MOVFF   Result, PORTC

    BCF     PORTD,0
    BZ      ZSET_ADD
    RETURN
ZSET_ADD:
    BSF     PORTD,0
    RETURN

; -- 3) XOR (PORTA ^ PORTB) --
SUB_XOR:
    MOVF    PORTA, W
    ANDLW   0x0F
    MOVWF   InA
    MOVF    PORTB, W
    ANDLW   0x0F                ; W = InB
    XORWF   InA, W              ; W = InA ^ InB  (Z set by XORWF)
    ANDLW   0x0F
    MOVWF   Result
    MOVFF   Result, PORTC

    BCF     PORTD,0
    BZ      ZSET_XOR
    RETURN
ZSET_XOR:
    BSF     PORTD,0
    RETURN

; -- 4) OR (PORTA | PORTB) --
SUB_OR:
    MOVF    PORTA, W
    ANDLW   0x0F
    MOVWF   InA
    MOVF    PORTB, W
    ANDLW   0x0F                ; W = InB
    IORWF   InA, W              ; W = InA | InB  (Z set by IORWF)
    ANDLW   0x0F
    MOVWF   Result
    MOVFF   Result, PORTC

    BCF     PORTD,0
    BZ      ZSET_OR
    RETURN
ZSET_OR:
    BSF     PORTD,0
    RETURN

; -- 5) AND (PORTA & PORTB) --
SUB_AND:
    MOVF    PORTA, W
    ANDLW   0x0F
    MOVWF   InA
    MOVF    PORTB, W
    ANDLW   0x0F                ; W = InB
    ANDWF   InA, W              ; W = InA & InB  (Z set by ANDWF)
    ANDLW   0x0F
    MOVWF   Result
    MOVFF   Result, PORTC

    BCF     PORTD,0
    BZ      ZSET_AND
    RETURN
ZSET_AND:
    BSF     PORTD,0
    RETURN

; -- 6) BCD convert nibble in PORTA[3:0] to packed BCD (0..15 -> 0x00..0x15) --
; Method: if value > 9 then add 6 -> yields packed BCD in Result.
SUB_BCD:
    MOVF    PORTA, W
    ANDLW   0x0F
    MOVWF   InA                 ; binary 0..15

    MOVLW   0x09
    CPFSGT  InA, 1              ; skip next if InA > 9
    GOTO    BCD_LESS10
    ; InA > 9
    MOVLW   0x06
    ADDWF   InA, F              ; InA += 6  (now packed BCD)
    MOVFF   InA, Result
    MOVFF   Result, PORTC

    BCF     PORTD,0
    BZ      ZSET_BCDG
    RETURN
ZSET_BCDG:
    BSF     PORTD,0
    RETURN

BCD_LESS10:
    MOVFF   InA, Result         ; already 0..9 -> same in BCD
    MOVFF   Result, PORTC

    BCF     PORTD,0
    BZ      ZSET_BCDL
    RETURN
ZSET_BCDL:
    BSF     PORTD,0
    RETURN

    END



