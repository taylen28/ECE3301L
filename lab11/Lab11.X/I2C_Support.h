void DS1621_Init(void);
void DS3231_Read_Time(void);
void DS3231_Setup_Time(void);
int DS1621_Read_Temp(void);
int DS1621_Read_Temp_Bad(void);

#define SCL_PIN 	PORTDbits.RD6
#define SCL_DIR 	TRISDbits.RD6

#define SDA_PIN 	PORTCbits.RC4
#define SDA_DIR 	TRISCbits.RC4