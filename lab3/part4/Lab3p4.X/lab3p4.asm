;========================================
; PIC18F4620  - Part 4 solution (WORKING)
; Counts 0..7 on PORTC with ~1s delay
;========================================
        LIST    P=18F4620, F=INHX32
        #include <p18f4620.inc>

;-------------------------
; Config bits
;-------------------------
        CONFIG  OSC = INTIO67
        CONFIG  WDT = OFF
        CONFIG  LVP = OFF
        CONFIG  BOREN = OFF

;-------------------------
; RAM variables (ACCESS bank)
;-------------------------
        CBLOCK  0x20
        Counter_L       ; inner delay counter
        Counter_H       ; outer delay counter
        Color_Value     ; 0..7
        Loop_Count      ; loop down from 8
        ENDC

;-------------------------
; Reset vector
;-------------------------
        ORG     0x0000
        GOTO    START

;-------------------------
; Init
;-------------------------
START:
        ; Put CPU on 8 MHz internal clock (IRCF=111), SCS=10 -> internal
        MOVLW   b'01110010'
        MOVWF   OSCCON, ACCESS

        ; All digital I/O, kill comparators (good hygiene even if not needed for RC)
        MOVLW   0x0F
        MOVWF   ADCON1, ACCESS
        MOVLW   0x07
        MOVWF   CMCON,  ACCESS

        ; I/O directions
        MOVLW   0xFF
        MOVWF   TRISA,  ACCESS      ; PORTA inputs (lab spec)
        CLRF    TRISB,  ACCESS      ; PORTB outputs
        CLRF    TRISC,  ACCESS      ; PORTC outputs
        CLRF    TRISD,  ACCESS      ; PORTD outputs

        CLRF    LATC,   ACCESS      ; clear output latch

;-------------------------
; while(1) { for(i=0;i<8;i++){ LATC=i; delay(); } }
;-------------------------
WHILE_LOOP:
        CLRF    Color_Value, ACCESS     ; i = 0
        MOVLW   0x08
        MOVWF   Loop_Count, ACCESS      ; count = 8

FOR_LOOP:
        MOVF    Color_Value, W, ACCESS
        MOVWF   LATC, ACCESS            ; drive RGB/LEDs (use LAT, not PORT)

        CALL    DELAY_ONE_SEC

        INCF    Color_Value, F, ACCESS  ; i++
        DECF    Loop_Count, F, ACCESS   ; count--
        BNZ     FOR_LOOP                ; repeat until count == 0

        BRA     WHILE_LOOP

;-------------------------
; ~1 second delay @ Fosc=8MHz (Fcy=2MHz, Tcy=0.5us)
; triple-nested loops (approximate; tweak if needed)
;-------------------------
DELAY_ONE_SEC:
        MOVLW   40
        MOVWF   Counter_H, ACCESS       ; Z = 40
DLY_Z:
        MOVLW   125
        MOVWF   Loop_Count, ACCESS      ; Y = 125
DLY_Y:
        MOVLW   200
        MOVWF   Counter_L, ACCESS       ; X = 200
DLY_X:
        DECFSZ  Counter_L, F, ACCESS
        BRA     DLY_X

        DECFSZ  Loop_Count, F, ACCESS
        BRA     DLY_Y

        DECFSZ  Counter_H, F, ACCESS
        BRA     DLY_Z

        RETURN

        END
