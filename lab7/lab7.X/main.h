//colors
#define OFF             0               // Defines OFF as decimal value 0
#define RED             1               // Defines RED as decimal value 1
#define GREEN           2               // Defines GREEN as decimal value 2
#define YELLOW          3               // Defines YELLOW as decimal value 3

#define Semester        0               // Type 0 for Sp, 1 for Fa
#define Year            24              // Type 2 digits year
#define Session_Number  1               // Type Session Number 1 through 5
#define Table_Number    01              // Type Table Number from 01 through 14

//switches
#define NSPED_SW PORTAbits.RA3
#define EWPED_SW PORTAbits.RA4
#define NSLT_SW PORTAbits.RA5
#define EWLT_SW PORTCbits.RC0

//outputs
#define NS_RED PORTAbits.RA1
#define NS_GREEN PORTAbits.RA2     
#define EW_RED PORTCbits.RC4
#define EW_GREEN PORTCbits.RC5
#define EWLT_RED PORTBbits.RB7
#define EWLT_GREEN PORTEbits.RE0
#define NSLT_RED PORTCbits.RC2
#define NSLT_GREEN PORTCbits.RC3
#define OFF 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define UPPER_7SEG PORTB
#define LOWER_7SEG PORTD
#define MODE_LED PORTEbits.RE1
#define SEC_LED PORTEbits.RE2


#define DP	        PORTbits.R??       // Defines Decimal Point as PORTD bit R??

#define mask_TFT	0x24 //step 7				// mask to protect the TFT signals

#define PEDESTRIAN_NS_WAIT  8
#define NS_WAIT             8
#define EW_LT_WAIT          8
#define PEDESTRIAN_EW_WAIT  8
#define EW_WAIT             8
#define NS_LT_WAIT          8

#define NIGHT_NS_WAIT       8
#define NIGHT_EW_LT_WAIT    8
#define NIGHT_EW_WAIT       8
#define NIGHT_NS_LT_WAIT    8

