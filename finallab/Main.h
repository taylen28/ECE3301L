void Set_D1_RGB(char duty_cycle);
void Set_D2_RGB(int rpm);
void Set_D3_RGB(char);

#define _XTAL_FREQ      8000000

#define Semester        ?               // Type 0 for Sp, 1 for Fa
#define Year            ??              // Type 2 digits year
#define Session_Number  ?               // Type Session Number 1 through 5
#define Table_Number    ??              // Type Table Number from 01 through 14

#define ACK             1
#define NAK             0

#define ACCESS_CFG      0xAC
#define START_CONV      0xEE
#define READ_TEMP       0xAA
#define CONT_CONV       0x02

#define KEY_PRESSED     PORT?bits.R??           // <-- Need to change the assignments here

#define Ch_Minus        0
#define Channel         1
#define Ch_Plus         2
#define Prev            3
#define Next            4
#define Play_Pause      5
#define Minus           6
#define Plus            7
#define EQ              8
#define Zero_Button     9






