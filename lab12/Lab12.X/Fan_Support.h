#ifndef FAN_SUPPORT_H
#define FAN_SUPPORT_H

typedef enum {
    OFF, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE
} colors;

#endif

int get_RPM();
void Toggle_Fan();
void Turn_Off_Fan();
void Turn_On_Fan();
void Increase_Speed();
void Decrease_Speed();
void do_update_pwm(char) ;
void Set_DC_RGB(int);
void Set_RPM_RGB(int);
void Set_TempC_RGB(signed char);
void Set_RGB_Color_D1(colors c);
void Set_RGB_Color_D2(colors c);



//D1 RGB pins
#define LED_R_D1 LATBbits.LATB3
#define LED_G_D1 LATBbits.LATB4
#define LED_B_D1 LATBbits.LATB5

// D2 RGB pins
#define LED_R_D2 LATAbits.LATA0
#define LED_G_D2 LATAbits.LATA1
#define LED_B_D2 LATAbits.LATA2

#define FAN_EN          PORTDbits.RD1
#define FAN_PWM         PORTCbits.RC2
#define FANON_LED       PORTDbits.RD4


