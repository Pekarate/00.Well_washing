/*

            FINAL PROGRAM FOR CELLSTAIN


controller: PIC18F4620
crystal   : 10Mhz
Software  : MPLAB IDE v8.92

configuration bits in configure
change this settings in the MPLAB IDE software itself.

OSC    - Oscillator Selection bit            - HS oscillator
WDT    - Watchdog Timer Enable bit           - WDT disabled
WDTPS  - Watchdog Timer Postscale Select bit - 1:1
PBADEN - PORTB A/D Enable bit                - PORTB<4:0> pins are configured as digital I/O on Reset
LVP    - Single-Supply ICSP Enable bit       - Single-Supply ICSP disabled

LCD DISPLAY USED HERE IS 16*2 LCD DISPLAY

THIS USES 2 NO OF A3967SLBT STEPPER MOTOR DRIVER CIRCUIT.

THIS PROGRAM MAINLY CONTROLLING TWO STEPPER MOTOR. 1,HEAD(ARM) MOTOR 2, CARRIAGE MOTOR

EN_STEPPER1=1; STOP  THE HAED-ARM MOTOR
EN_STEPPER1=0; START THE HAED-ARM MOTOR

EN_STEPPER=0;  STOP  THE CARRIAGE MOTOR
EN_STEPPER=1;  START THE CARRIAGE MOTOR

HEAD MOTOR ATTACHED WITH THE ARM AND THE ARM WILL HOLD THE GLASS PLATES TO BE DIPPED IN SOLUTIONS.
 IT MOVES UP AND DOWN AND AGGETATE IN EACH POSITION.

CARRIAGE  MOTOR MOVES THE HEAD TO PROPER POSITION, THIS CONTAIN
6 WELL(BUCKET) POSITION AND 1 TEMPERATURE WELL TOTALLY 7 POSITION EACH IS PLACE EQUAL DISTANCE
OF APPROXIMATLY 5cm. MOVING  TO DIFFERENT POSITION CAN BE PROGRAMED

THIS CELL STAIN PROGRAM CAN CAPABLE TO STORE 5 DIFFERENT PROGRAM WITH MAXIMUM OF 6 STEPS.

EACH STEP HAVING ONE  LOWER LEVEL, IN EACH LEVEL IT CAN HOLD AND SHAKE ,
IN PROGRAM IT CAN BE MENTIONED HOW LONG IT SHOULD BE IN UPPER LEVEL (HOLDING TIME AND SHAKING TIME)MAXIMUM HOLDING AND SHAKING TIME IS 30 MINUTES 30 SECONDS ON BOTH THE LEVELS
AND LOWER LEVEL (HOLDING TIME AND SHAKING TIME).

THIS CAN ALSO BE PROGRAMED NO OF STEPS IN EACH PROGRAM AND THE ORDER OF THE STEPS,
TEMPERATURE (RANGE 40-60 DEGREE) FOR DRYING THE GLASS PLATE AND DRY TIME (RANGE 0-240 SECONDS)

WORKING WITH PROGRAM :
STEP BY SPEP PROGRAMMING THE INSTRUMENT

WHEN THE INSTRUMENT POWER ON DISPLAY SHOW INITIALIZTION IN THE MEAN TIME BOTH THE MOTOR COMES
TO HOME POSITIONIF ITS ALREADY IN HOME POSITION IT MAKE SURE AGAIN ITS IN HOME POSITION BY
2 INDIVIDUAL OPTICAL SENSOR.

THEN DISPLAY SHOW

RUN OLD PROGRAM IN LINE 1

ENTER NEW PROGRAM IN LINE 2

THIS CAN BE SELECTED USING UP/DOWN BUTTON AND PRESS OK BUTTON

FIRST ENTER TO ENTER NEW PROGRAM MODE IT WILL ASK PROGRAM NO USE UP/DOWN KEY TO SET PROGRAM NO AND PRESS OK.

THEN IT ASK FOR LEVEL WAITING TIME SHAKING TIME IN EACH STEPS (TOTAL 6 STEPS) AFTER SETTING THIS PRESS OK

THEN IT ASKS FOR TEMPERATURE IN CELSIUS ENTER VALUE (40-60)USE UP/DOWN KEY AND PRESS OK.

THEN IT ASKS FOR DRY TIME IN SECONDS ENTER VALUE (1-240)USE UP/DOWN KEY AND PRESS OK.

THEN IT COMES TO HOME PAGE NOW SELECT RUN OLD PROGRAM

IT ASK FOR PROGRAM NO SELECT THE PROGRAM YOU WANT TO RUN AND PRESS OK THEN IT STARTS THE PROCESS.

*/

#include <htc.h>

#define UP PORTBbits.RB0      // key
#define DOWN PORTBbits.RB1    // key
#define OK PORTBbits.RB2      // key
#define BACK PORTBbits.RB3    // key
#define DC_HOME PORTAbits.RA4 // carriage motor home position sensor
#define HOME PORTAbits.RA5    // head(ARM) motor home position sensor

// LCD used here is 16*2 graphical LCD
// PINS FOR LCD

#define RS PORTEbits.RE0
#define RW PORTEbits.RE1
#define EN PORTEbits.RE2
#define dataport PORTD // LCD DATA PORTD

#define STEP PORTBbits.RB7       // PULSE FOR CARRIAGE MOTOR
#define DIR PORTBbits.RB6        // DIRECTION FOR CARRIAGE MOTOR
#define EN_STEPPER PORTBbits.RB5 // ENABLE AND DISABLE THE CARRIAGE MOTOR

#define HEATER PORTCbits.RC7

#define STEP1 PORTCbits.RC0       // PULE FOR HAED(ARM)MOTOR
#define DIR1 PORTCbits.RC1        // DIRECTION FOR HEAD(ARM) MOTOR
#define EN_STEPPER1 PORTCbits.RC2 // ENABLE AND DISABLE(START/STOP)HAED(ARM) MOTOR

#define HIGH 1
#define LOW 0
#define slow 0
#define speed 1

float temperature;
int shake = 0, adc_value, set_temperature;
int line = 1, up = 0, ok = 0, back = 0, one2, ten2, one3, ten3, hun4, ten4, one4, tem;
unsigned int s = 5, temp = 0, z1 = 0, z2 = 0, temp_time = 0, time1, time2, time3, time4;
unsigned int minute1 = 0, minute2 = 0, step = 0, second1 = 0, second2 = 0, up1, up2, up3, up4, t = 1, second3 = 0, second4 = 0, minute3 = 0, minute4 = 0, data = 0;
void LCD_Display(unsigned char *string);
void wrdata(unsigned char data);
void wrcomm(unsigned char command);
void motor(int b, int d);
void motor1(int b, int d);
msdelay(unsigned int a);
void delay_ms(int a);
int programme[32] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf};

int temper[5] = {1, 2, 3, 4, 5};
int drytime[5] = {6, 7, 8, 9, 10};
int wait1[5] = {11, 12, 13, 14, 15};
int shake1[5] = {16, 17, 18, 19, 20};
int wait2[5] = {21, 22, 23, 24, 25};
int shake2[5] = {26, 27, 28, 29, 30};
int wait3[5] = {31, 32, 33, 34, 35};
int shake3[5] = {36, 37, 38, 39, 40};
int wait4[5] = {41, 42, 43, 44, 45};
int shake4[5] = {46, 47, 48, 49, 50};
int wait5[5] = {51, 52, 53, 54, 55};
int shake5[5] = {56, 57, 58, 59, 60};
int wait6[5] = {61, 62, 63, 64, 65};
int shake6[5] = {66, 67, 68, 69, 70};
int wait7[5] = {71, 72, 73, 74, 75};
int shake7[5] = {76, 77, 78, 79, 80};
int wait8[5] = {81, 82, 83, 84, 85};
int shake8[5] = {86, 87, 88, 89, 90};
int wait9[5] = {91, 92, 93, 94, 95};
int shake9[5] = {96, 97, 98, 99, 100};
int wait10[5] = {101, 102, 103, 104, 105};
int shake10[5] = {106, 107, 108, 109, 110};

int sec1[5] = {111, 112, 113, 114, 115};
int sec2[5] = {116, 117, 118, 119, 120};
int sec3[5] = {121, 122, 123, 124, 125};
int sec4[5] = {126, 127, 128, 129, 130};
int sec5[5] = {131, 132, 133, 134, 135};
int sec6[5] = {136, 137, 138, 139, 140};
int sec7[5] = {141, 142, 143, 144, 145};
int sec8[5] = {146, 147, 148, 149, 150};
int sec9[5] = {151, 152, 153, 154, 155};
int sec10[5] = {156, 157, 158, 159, 160};
int sec11[5] = {161, 162, 163, 164, 165};
int sec12[5] = {166, 167, 168, 169, 170};
int sec13[5] = {171, 172, 173, 174, 175};
int sec14[5] = {176, 177, 178, 179, 180};
int sec15[5] = {181, 182, 183, 184, 185};
int sec16[5] = {186, 187, 188, 189, 190};
int sec17[5] = {191, 192, 193, 194, 195};
int sec18[5] = {196, 197, 198, 199, 200};
int sec19[5] = {201, 202, 203, 204, 205};
int sec20[5] = {206, 207, 208, 209, 210};

//---------------------------------------------------------------------
// Function:   delay for 1ms
// Parameters: - timer0 configuration
//---------------------------------------------------------------------

MSDELAY(unsigned long int k)
{
    for (; k > 0; k--)
    {
        T0CON = 0X07;
        TMR0H = 0Xff; // delay of 1ms
        TMR0L = 0Xf5;
        TMR0ON = 1;
        while (!TMR0IF)
            ;
        TMR0IF = 0;
        TMR0ON = 0;
    }
}

//---------------------------------------------------------------------
// Function:   delay for 2us
// Parameters: - timer1 configuration
//---------------------------------------------------------------------

delay(unsigned long int a)
{
    for (; a > 0; a--)
    {
        T1CON = 0X80;
        TMR1H = 0XFF; // delay of 2us
        TMR1L = 0XFA;
        TMR1ON = 1;
        while (!TMR1IF)
            ;
        TMR1IF = 0;
        TMR1ON = 0;
    }
}

//---------------------------------------------------------------------
// Function:   delay_ms
// Parameters: - small approximate 1ms delay
//
//---------------------------------------------------------------------
void delay_ms(int a)
{
    int b = a * 5;
    for (; a > 0; a--)
        for (; b > 0; b--)
            ;
}
//---------------------------------------------------------------------
// Function:   temp_check_1
// Parameters: -
// This is to measure the temperature in chamber_1
// it will maintain temperature in specific limit by switching ON and OFF
//  HEATING ELEMENT.
//---------------------------------------------------------------------

void temp_check()
{
    set_temperature = temp;
    ADRES = 0X00;
    ADCON0 = 0x01; // AN0 channel_0 is enabled here.
    ADCON1 = 0X0E;
    ADCON2 = 0X80;

    ADCON0bits.GO = 1;
    while (ADCON0bits.DONE)
        ;

    adc_value = ADRES;
    temperature = adc_value * .488; // Lm35 ic is used ./deg C change o/p is 10 mv .
    tem = temperature;
    ten2 = (int)tem / 10;
    one2 = (int)tem % 10;
    ten3 = (int)set_temperature / 10;
    one3 = (int)set_temperature % 10;

    if (temperature > (set_temperature))
    {
        delay(2000);
        HEATER = LOW;
    }

    if (temperature < (set_temperature))
    {
        delay(2000);
        HEATER = HIGH;
    }
}
//---------------------------------------------------------------------
// Function:   shaker
// THIS IS USE TO MOVES THE ARM UP AND DOWN IN TWO DIFFERENT DISTANCE
//---------------------------------------------------------------------

shaker(unsigned int a, unsigned int d)
{
    for (; a > 0; a--)
    {
        T3CON = 0X80;
        TMR3H = 0XF6; // delay of 1ms
        TMR3L = 0X3B;
        TMR3ON = 1;
        while (!TMR3IF)
        {
            if (d == 1 && shake == 1)
            {
                DIR1 = HIGH;
                motor1(100, d);
                DIR1 = LOW;
                motor1(100, d);
            }
            if (d == 0 && shake == 1)
            {
                DIR1 = HIGH;
                motor1(400, d);
                DIR1 = LOW;
                motor1(400, d);
            }
        }
        TMR3IF = 0;
        TMR3ON = 0;
    }
}
//---------------------------------------------------------------------
// Function:   motor_delay1
// THIS IS USE TO RUN THE MOTOR SLOWLY IN SPECIFIC MENTIONED TIME
//---------------------------------------------------------------------

motor_delay1(unsigned int time)
{
    for (int j = 0; j < time; j++)
    {
        TMR2 = 0x63;
        T2CON = 0x07;
        while (!TMR2IF)
        {
            shaker(1, slow);
        }
        TMR2IF = 0;
        TMR2ON = 0;
    }
}

//---------------------------------------------------------------------
// Function:   motor_delay2
// THIS IS USE TO RUN THE MOTOR SPEEDLY IN SPECIFIC MENTIONED TIME
//---------------------------------------------------------------------
motor_delay2(int time)
{
    for (int j = 0; j < time; j++)
    {
        TMR2 = 0x63;
        T2CON = 0x07;
        while (!TMR2IF)
        {
            shaker(1, speed);
        }
        TMR2IF = 0;
        TMR2ON = 0;
    }
}

//---------------------------------------------------------------------
// Function:   dry
// THIS IS TO STAY IN TEMPERATURE CHAMBER WITH MENTIONED TIME AND
// MONITOR , MAINTAIN & DISPLAY ACTUAL TEMPERATURE
//---------------------------------------------------------------------
dry(unsigned int a)
{
    for (; a > 0; a--)
    {
        for (int g = 5; g > 0; g--)
        {
            T3CON = 0XB0;
            TMR3H = 0X0B; // delay of 1ms
            TMR3L = 0XDB;
            TMR3ON = 1;
            while (!TMR3IF)
            {
                temp_check();
                wrcomm(0x01);
                wrcomm(0x80);
                LCD_Display(" Temp:");
                wrcomm(0x86);
                wrdata(ten2 + 48);
                wrdata(one2 + 48);
                wrdata('C');
                wrcomm(0x8B);
                wrdata(hun4 + 48);
                wrdata(ten4 + 48);
                wrdata(one4 + 48);
                wrdata('s');
                wrcomm(0xc0);
                LCD_Display(" SetTemp ");
                wrdata(ten3 + 48);
                wrdata(one3 + 48);
                wrdata('C');
            }
            TMR3IF = 0;
            TMR3ON = 0;
        }
    }
}
//---------------------------------------------------------------------
// Function  :   msdelay
// Parameters:   EXACT 1ms delay
// timer3 config
//---------------------------------------------------------------------
msdelay(unsigned int a)
{
    for (; a > 0; a--)
    {
        T3CON = 0X80;
        TMR3H = 0XF6; // delay of 1ms
        TMR3L = 0X3B;
        TMR3ON = 1;
        while (!TMR3IF)
            ;
        TMR3IF = 0;
        TMR3ON = 0;
    }
}
//---------------------------------------------------------------------
// Function  :   motor1
// THIS FUNCTION IS TO MOVE THE HEAD MOTOR TO MOVE ARM UP AND DOWN
// ACTUALLY THIS MOTOR INITAILLY MOVES SLOWLY WITH SMALL FREQUENCY AND REDUCED STEP BY STEP AND SETTELD IN PARTICULAR SPEED
//
//---------------------------------------------------------------------

void motor1(int b, int d)
{

    for (int c = 15; c > 0; c--)
    {
        STEP1 = 1;
        delay(80);
        STEP1 = 0;
        delay(80);
    }

    for (int c = 15; c > 0; c--)
    {
        STEP1 = 1;
        delay(75);
        STEP1 = 0;
        delay(75);
    }
    for (int c = 15; c > 0; c--)
    {
        STEP1 = 1;
        delay(70);
        STEP1 = 0;
        delay(70);
    }
    for (int c = 15; c > 0; c--)
    {
        STEP1 = 1;
        delay(70);
        STEP1 = 0;
        delay(70);
    }

    for (int c = 15; c > 0; c--)
    {
        STEP1 = 1;
        delay(65);
        STEP1 = 0;
        delay(65);
    }

    for (int c = 15; c > 0; c--)
    {
        STEP1 = 1;
        delay(60);
        STEP1 = 0;
        delay(60);
    }
    for (int c = 15; c > 0; c--)
    {
        STEP1 = 1;
        delay(55);
        STEP1 = 0;
        delay(55);
    }

    for (int c = 15; c > 0; c--)
    {

        STEP1 = 1;
        delay(40);
        STEP1 = 0;
        delay(40);
    }
    for (; b > 0; b--)
    {
        if (!HOME)
            b = 0;
        STEP1 = 1;
        delay(30);
        STEP1 = 0;
        delay(30);
    }
}
//-----------------------------------------------------------------
// 0000000000000000000000000000000000000000000000000000000000000000000000000000000000
// Function: home_pos
// THIS IS TO MAKE THE HEAD(ARM)MOTOR TO COME HOME POSITION IN INITIALIZATION USING OPTICAL SENSOR
//
// 0000000000000000000000000000000000000000000000000000000000000000000000000000000000

void home_pos()
{
    for (int t = 0; t < 1; t++)
    {
        if (!HOME)
        {
            DIR1 = HIGH;
            motor1(1000, speed);
        }
        while (HOME)
        {
            DIR1 = LOW;
            motor1(25000, speed);
        }
    }
}
//---------------------------------------------------------------------
// Function  :   motor
// THIS FUNCTION IS TO MOVE THE CARRIAGE MOTOR TO MOVE HEAD TO PROPER POSITION FORWARD AND BACKWARD
// ACTUALLY THIS MPOTOR INITAILLY MOVES SLOWLY WITH SMALL FREQUENCY AND REDUCED STEP BY STEP AND SETTELD IN PARTICULAR SPEED
void motor(int b, int d)
{
    for (int c = 15; c > 0; c--)
    {
        STEP = 1;
        delay(10);
        STEP = 0;
        delay(280);
    }

    for (int c = 15; c > 0; c--)
    {
        STEP = 1;
        delay(10);
        STEP = 0;
        delay(270);
    }
    for (int c = 15; c > 0; c--)
    {
        STEP = 1;
        delay(10);
        STEP = 0;
        delay(230);
    }

    for (int c = 15; c > 0; c--)
    {
        STEP = 1;
        delay(10);
        STEP = 0;
        delay(200);
    }

    for (int c = 15; c > 0; c--)
    {
        STEP = 1;
        delay(10);
        STEP = 0;
        delay(170);
    }

    for (int c = 15; c > 0; c--)
    {
        STEP = 1;
        delay(10);
        STEP = 0;
        delay(150);
    }
    b = b - 90; // NO OF PULSE PREVIOUSLY GIVEN TO THE MOTOR SUBTARCTED HERE TO STOP ACCURATE POSITION EACH TIME
                // 15PULSES WITH 280,270,230,200,170,150 TOTAL 90 STEPS SUBTRACTED.
    for (; b > 0; b--)
    {
        if (!DC_HOME)
            b = 0;
        STEP = 1;
        delay(10);
        STEP = 0;
        delay(130);
    }
}

// 0000000000000000000000000000000000000000000000000000000000000000000000000000000000
// Function: dc_homepos
//  THIS IS TO MAKE THE CARRIAGE MOTOR TO COME HOME POSITION IN INITIALIZATION USING OPTICAL SENSOR
//
// 0000000000000000000000000000000000000000000000000000000000000000000000000000000000

dc_homepos()
{
    if (!DC_HOME)
    {
        DIR = HIGH;
        motor(200, speed);
    }
    while (DC_HOME)
    {
        DIR = LOW;
        motor(10000, speed);
        msdelay(1000);
    }
}
// 0000000000000000000000000000000000000000000000000000000000000000000000000000000000
// Function: read_eeprom
//  THIS IS TO READ VALUES FROM EEPROM WITH MENTIONED ADDRESS
//
// 0000000000000000000000000000000000000000000000000000000000000000000000000000000000

char read_eeprom(unsigned int address)
{
    EEADR = address;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.RD = 1;
    return (EEDATA);
}
// 0000000000000000000000000000000000000000000000000000000000000000000000000000000000
// Function: write_eeprom
//  THIS IS TO WRITE VALUES TO EEPROM WITH MENTIONED ADDRESS AND DATA
//
// 0000000000000000000000000000000000000000000000000000000000000000000000000000000000

void write_eeprom(unsigned int address, unsigned int data)
{
    unsigned char INTCON_SAVE;

    EEADR = address;
    EEDATA = data;

    EECON1bits.EEPGD = 0; // 0 = Access data EEPROM memory
    EECON1bits.CFGS = 0;  // 0 = Access Flash program or DATA EEPROM memory
    EECON1bits.WREN = 1;  // enable writes to internal EEPROM
    INTCON_SAVE = INTCON;
    INTCONbits.GIE = 1;
    EECON2 = 0x55;
    EECON2 = 0xaa;
    EECON1bits.WR = 1;
    while (PIR2bits.EEIF == 0)
        ;
    EECON1bits.WR = 0;
    PIR2bits.EEIF = 0;
    EECON1bits.WREN = 0;
}

// 0000000000000000000000000000000000000000000000000000000000000000000000000000000000
// Function: wrcomm
//  THIS IS TO WRITE COMMAND TO LCD DISPLAY
//
// 0000000000000000000000000000000000000000000000000000000000000000000000000000000000
void wrcomm(unsigned char command)
{
    dataport = command;
    RS = 0;
    RW = 0;
    EN = 0;
    delay_ms(2);
    EN = 1;
    delay_ms(10);
}
// 0000000000000000000000000000000000000000000000000000000000000000000000000000000000
// Function: LCD_intlz
//  THIS IS TO give initialize command the lcd
//
// 0000000000000000000000000000000000000000000000000000000000000000000000000000000000
void LCD_intlz(void)
{
    wrcomm(0x0C); // Display on Cursor off //

    wrcomm(0x01); // clear  previous DDRAM content//

    wrcomm(0x38); // Function Set for 8-bit, 2 Line, 5x7 Dots //

    wrcomm(0x80); // Force cursor to the beginning of 1st line//
                  // wrcomm(0xC0);               //Force cursor to the beginning of 1st line//
                  // wrcomm(0x94);               //Force cursor to the beginning of 1st line//
                  // wrcomm(0xD4);               //Force cursor to the beginning of 1st line//
    //   wrcomm(0x06);               //Right entry Mode//

    //   wrcomm(0x0e);               // Cursor Blinking On //
    wrcomm(0x3c);
    wrcomm(0x0f);
}
// 0000000000000000000000000000000000000000000000000000000000000000000000000000000000
// Function: wrdata
//  THIS IS TO write data to the lcd
// 0000000000000000000000000000000000000000000000000000000000000000000000000000000000
void wrdata(unsigned char data)
{
    dataport = data;
    RS = 1;
    RW = 0;
    EN = 0;
    delay_ms(2);
    EN = 1;
    delay_ms(20);
}

// 0000000000000000000000000000000000000000000000000000000000000000000000000000000000
// Function: LCD_Display
//  THIS IS TO convert the string into individual character to be display
// 0000000000000000000000000000000000000000000000000000000000000000000000000000000000
void LCD_Display(unsigned char *string)
{

    while (*string)
        wrdata(*string++);
}
// 0000000000000000000000000000000000000000000000000000000000000000000000000000000000
// Function: get_parameter
//  THIS IS TO read parameters from the eeprom accordingly to the each program
//  parameters like shaking time, waiting time, temperature, drytime,
//  no of steps on each program, order of position in each program
// 0000000000000000000000000000000000000000000000000000000000000000000000000000000000
get_parameter()
{
    for (; step <= 4; step++)
    {
        // each step in each program have different parameters
        // level waiting time, shaking time and this if(conditons) used to give starting address for these parametrs for each step in each program
        // using the above address read values from eeprom
        if (step == 0)
        {
            second1 = read_eeprom(sec1[s]);
            second2 = read_eeprom(sec2[s]);
            minute1 = read_eeprom(wait1[s]);
            minute2 = read_eeprom(shake1[s]);

            second3 = read_eeprom(sec3[s]);
            second4 = read_eeprom(sec4[s]);
            minute3 = read_eeprom(wait2[s]);
            minute4 = read_eeprom(shake2[s]);
        }
        if (step == 1)
        {
            second1 = read_eeprom(sec5[s]);
            second2 = read_eeprom(sec6[s]);
            minute1 = read_eeprom(wait3[s]);
            minute2 = read_eeprom(shake3[s]);

            second3 = read_eeprom(sec7[s]);
            second4 = read_eeprom(sec8[s]);
            minute3 = read_eeprom(wait4[s]);
            minute4 = read_eeprom(shake4[s]);
        }
        if (step == 2)
        {
            second1 = read_eeprom(sec9[s]);
            second2 = read_eeprom(sec10[s]);
            minute1 = read_eeprom(wait5[s]);
            minute2 = read_eeprom(shake5[s]);

            second3 = read_eeprom(sec11[s]);
            second4 = read_eeprom(sec12[s]);
            minute3 = read_eeprom(wait6[s]);
            minute4 = read_eeprom(shake6[s]);
        }
        if (step == 3)
        {
            second1 = read_eeprom(sec13[s]);
            second2 = read_eeprom(sec14[s]);
            minute1 = read_eeprom(wait7[s]);
            minute2 = read_eeprom(shake7[s]);

            second3 = read_eeprom(sec15[s]);
            second4 = read_eeprom(sec16[s]);
            minute3 = read_eeprom(wait8[s]);
            minute4 = read_eeprom(shake8[s]);
        }
        if (step == 4)
        {
            second1 = read_eeprom(sec17[s]);
            second2 = read_eeprom(sec18[s]);
            minute1 = read_eeprom(wait9[s]);
            minute2 = read_eeprom(shake9[s]);

            second3 = read_eeprom(sec19[s]);
            second4 = read_eeprom(sec20[s]);
            minute3 = read_eeprom(wait10[s]);
            minute4 = read_eeprom(shake10[s]);
        }
        // calculate total time in seconds for each operation

        time2 = minute1 * 60 + second1; // time2 variable have the  waiting time
        time1 = minute2 * 60 + second2; // time1 variable have the  shaking time
        time4 = minute3 * 60 + second3; // time4 variable have the  waiting time
        time3 = minute4 * 60 + second4; // time2 variable have the  shaking time

        // these address is to read temperature for each program
        temp_time = read_eeprom(drytime[s]);
        temp = read_eeprom(temper[s]);
        EN_STEPPER = 1;
        temp_check(); // heat element ON and starts controll the temperature
        //---------------------------------------------------------------------------------------------------------\
// for 1st step in each proram finds the position and moves the head in appropriate position
        if (step == 0)
        {
            // dc motor forward direction
            wrcomm(0X01);
            wrcomm(0X80);
            LCD_Display(" PROCESSING");
            wrcomm(0xc0);
            LCD_Display(" POSITION: ");
            wrdata(step + 48);
            EN_STEPPER = 1;
            DIR = HIGH;
            motor(310 * step, speed); // 310 is the value to move the head about 5cm
            EN_STEPPER = 1;           // if this value increases distance increase if value decrease distance decrease
            msdelay(1000);
        }
        if (step != 0) // this is 2nd step to 6 no of step moves head in forward direction
        {
            wrcomm(0X01);
            wrcomm(0X80);
            LCD_Display(" PROCESSING");
            wrcomm(0xc0);
            LCD_Display(" POSITION: ");
            wrdata(step + 48);
            msdelay(1000);
            EN_STEPPER = 0;
            DIR = LOW; // dc motor Reverse direction
            motor(310 * (5 - step), speed);
            EN_STEPPER = 1;
            msdelay(1000);
        }
        //-----------------------------------------------------------------------------------------------------------------------

        EN_STEPPER1 = 0;
        DIR1 = HIGH;
        motor1(3500, speed); // 3500 is value to move from arm home position to  level,if this value increases distance increase if value decrese distance decrease
        shake = 1;
        motor_delay1(time1);
        EN_STEPPER1 = 1;
        motor_delay1(time2);
        EN_STEPPER1 = 0;
        DIR1 = LOW;
        home_pos();
        wrcomm(0X01);
        wrcomm(0X80);
        LCD_Display(" PROCESSING");
        wrcomm(0xc0);
        LCD_Display(" POSITION: ");
        wrdata(6 + 48);
        EN_STEPPER = 0;
        DIR = HIGH; // dc motor forward  direction
        motor(310 * (5 - step), speed);
        EN_STEPPER = 1;
        EN_STEPPER1 = 0;
        DIR1 = HIGH;
        motor1(3500, speed); // 3500 is value to move from arm home position to level,if this value increases distance increase if value decrese distance decrease
        shake = 1;
        motor_delay1(time3);
        EN_STEPPER1 = 1;
        motor_delay1(time4);
        EN_STEPPER1 = 0;
        DIR1 = LOW;
        home_pos();
        EN_STEPPER = 1;
    }
    wrcomm(0X01);
    wrcomm(0X80);
    LCD_Display(" DRY POSITION");
    DIR = HIGH;
    EN_STEPPER = 0;
    motor(310 * 1, speed); // as final step this moves head to temperature well ie 7th position.
}
// 0000000000000000000000000000000000000000000000000000000000000000000000000000000000
// Function: set_parameter
//  THIS IS TO set different parameters and write to the eeprom accordingly to the each program
//  parameters like shaking time, waiting time, temperature, drytime,
//  no of steps on each program, order of position in each program
// 0000000000000000000000000000000000000000000000000000000000000000000000000000000000
set_parameter()
{
    // each step in each program values to be set and the values to be writestep by step in eeprom with proper address
    // this part of program assigns the proper address to particular parameter values to each program
    for (; step <= 4; step++)
    {
        MSDELAY(500);
        if (step == 0)
        {

            second1 = read_eeprom(sec1[s]);
            second2 = read_eeprom(sec2[s]);
            minute1 = read_eeprom(wait1[s]);
            minute2 = read_eeprom(shake1[s]);

            second3 = read_eeprom(sec3[s]);
            second4 = read_eeprom(sec4[s]);
            minute3 = read_eeprom(wait2[s]);
            minute4 = read_eeprom(shake2[s]);
        }
        if (step == 1)
        {
            second1 = read_eeprom(sec5[s]);
            second2 = read_eeprom(sec6[s]);
            minute1 = read_eeprom(wait3[s]);
            minute2 = read_eeprom(shake3[s]);

            second3 = read_eeprom(sec7[s]);
            second4 = read_eeprom(sec8[s]);
            minute3 = read_eeprom(wait4[s]);
            minute4 = read_eeprom(shake4[s]);
        }
        if (step == 2)
        {
            second1 = read_eeprom(sec9[s]);
            second2 = read_eeprom(sec10[s]);
            minute1 = read_eeprom(wait5[s]);
            minute2 = read_eeprom(shake5[s]);

            second3 = read_eeprom(sec11[s]);
            second4 = read_eeprom(sec12[s]);
            minute3 = read_eeprom(wait6[s]);
            minute4 = read_eeprom(shake6[s]);
        }
        if (step == 3)
        {
            second1 = read_eeprom(sec13[s]);
            second2 = read_eeprom(sec14[s]);
            minute1 = read_eeprom(wait7[s]);
            minute2 = read_eeprom(shake7[s]);

            second3 = read_eeprom(sec15[s]);
            second4 = read_eeprom(sec16[s]);
            minute3 = read_eeprom(wait8[s]);
            minute4 = read_eeprom(shake8[s]);
        }
        if (step == 4)
        {
            second1 = read_eeprom(sec17[s]);
            second2 = read_eeprom(sec18[s]);
            minute1 = read_eeprom(wait9[s]);
            minute2 = read_eeprom(shake9[s]);

            second3 = read_eeprom(sec19[s]);
            second4 = read_eeprom(sec20[s]);
            minute3 = read_eeprom(wait10[s]);
            minute4 = read_eeprom(shake10[s]);
        }
        wrcomm(0x01);
        wrcomm(0x80);
        LCD_Display(" WAIT-");
        up2 = minute1 / 10;
        up3 = minute1 % 10;
        wrdata(up2 + 48);
        wrdata(up3 + 48);
        LCD_Display("m:");
        up2 = second1 / 10;
        up3 = second1 % 10;
        wrdata(up2 + 48);
        wrdata(up3 + 48);
        LCD_Display("s");
        wrcomm(0xc0);
        LCD_Display(" SHAKE-");
        up2 = minute2 / 10;
        up3 = minute2 % 10;
        wrdata(up2 + 48);
        wrdata(up3 + 48);
        LCD_Display("m:");
        up2 = second2 / 10;
        up3 = second2 % 10;
        wrdata(up2 + 48);
        wrdata(up3 + 48);
        LCD_Display("s");
        wrcomm(0x8f);
        wrdata(step + 48);
        up2 = minute1 / 10;
        up3 = minute1 % 10;
        while (ok <= 7)
        {
            t = 6;
            while (ok == 0)
            {
                wrcomm(programme[t]);
                if (!UP)
                {
                    MSDELAY(300);
                    up1 = 9;
                    t++;
                    if (t > 7)
                    {
                        t = 6;
                    }
                }
                if (!DOWN)
                {
                    MSDELAY(300);
                    up1++;
                    if (up1 > 9)
                    {
                        up1 = 0;
                    }
                    wrdata(up1 + 48);
                    if (t == 6)
                    {
                        up2 = up1;
                    }
                    if (t == 7)
                    {
                        up3 = up1;
                    }
                }
                if (!OK)
                {
                    MSDELAY(300);
                    ok++;
                }
            }
            minute1 = ((up2 * 10) + (up3 * 1));
            up2 = second1 / 10;
            up3 = second1 % 10;
            t = 10;
            while (ok == 1 && BACK)
            {
                wrcomm(programme[t]);
                if (!UP)
                {
                    MSDELAY(300);
                    up1 = 9;
                    t++;
                    if (t > 11)
                    {
                        t = 10;
                    }
                }
                if (!DOWN)
                {
                    MSDELAY(300);
                    up1++;
                    if (up1 > 9)
                    {
                        up1 = 0;
                    }
                    wrdata(up1 + 48);
                    if (t == 10)
                    {
                        up2 = up1;
                    }
                    if (t == 11)
                    {
                        up3 = up1;
                    }
                }
                if (!OK)
                {
                    MSDELAY(300);
                    ok++;
                }
                if (!BACK)
                {
                    ok = 0;
                    MSDELAY(300);
                }
            }
            second1 = ((up2 * 10) + (up3 * 1));
            up2 = minute2 / 10;
            up3 = minute2 % 10;
            t = 23;
            while (ok == 2 && BACK)
            {
                wrcomm(programme[t]);
                if (!UP)
                {
                    MSDELAY(300);
                    up1 = 9;
                    t++;
                    if (t > 24)
                    {
                        t = 23;
                    }
                }
                if (!DOWN)
                {
                    MSDELAY(300);
                    up1++;
                    if (up1 > 9)
                    {
                        up1 = 0;
                    }
                    wrdata(up1 + 48);
                    if (t == 23)
                    {
                        up2 = up1;
                    }
                    if (t == 24)
                    {
                        up3 = up1;
                    }
                }
                if (!OK)
                {
                    MSDELAY(300);
                    ok++;
                }
                if (!BACK)
                {
                    ok = 1;
                    MSDELAY(300);
                }
            }
            minute2 = ((up2 * 10) + (up3 * 1));
            up2 = second2 / 10;
            up3 = second2 % 10;
            t = 27;
            while (ok == 3 && BACK)
            {
                wrcomm(programme[t]);
                if (!UP)
                {
                    MSDELAY(300);
                    up1 = 9;
                    t++;
                    if (t > 28)
                    {
                        t = 27;
                    }
                }
                if (!DOWN)
                {
                    MSDELAY(300);
                    up1++;
                    if (up1 > 9)
                    {
                        up1 = 0;
                    }
                    wrdata(up1 + 48);
                    if (t == 27)
                    {
                        up2 = up1;
                    }
                    if (t == 28)
                    {
                        up3 = up1;
                    }
                }
                if (!OK)
                {
                    MSDELAY(300);
                    ok++;
                    wrcomm(programme[31]);
                    wrdata(6 + 48);
                    wrcomm(programme[31]);
                }
                if (!BACK)
                {
                    MSDELAY(300);
                    ok = 2;
                }
            }
            second2 = ((up2 * 10) + (up3 * 1));
            wrcomm(0x80);
            LCD_Display(" WAIT-");
            up2 = minute3 / 10;
            up3 = minute3 % 10;
            wrdata(up2 + 48);
            wrdata(up3 + 48);
            LCD_Display("m:");
            up2 = second3 / 10;
            up3 = second3 % 10;
            wrdata(up2 + 48);
            wrdata(up3 + 48);
            LCD_Display("s");
            wrcomm(0xc0);
            LCD_Display(" SHAKE-");
            up2 = minute4 / 10;
            up3 = minute4 % 10;
            wrdata(up2 + 48);
            wrdata(up3 + 48);
            LCD_Display("m:");
            up2 = second4 / 10;
            up3 = second4 % 10;
            wrdata(up2 + 48);
            wrdata(up3 + 48);
            LCD_Display("s");
            up2 = minute3 / 10;
            up3 = minute3 % 10;
            t = 6;
            while (ok == 4)
            {
                wrcomm(programme[t]);
                if (!UP)
                {
                    MSDELAY(300);
                    up1 = 9;
                    t++;
                    if (t > 7)
                    {
                        t = 6;
                    }
                }
                if (!DOWN)
                {
                    MSDELAY(300);
                    up1++;
                    if (up1 > 9)
                    {
                        up1 = 0;
                    }
                    wrdata(up1 + 48);
                    if (t == 6)
                    {
                        up2 = up1;
                    }
                    if (t == 7)
                    {
                        up3 = up1;
                    }
                }
                if (!OK)
                {
                    MSDELAY(300);
                    ok++;
                }
            }
            minute3 = ((up2 * 10) + (up3 * 1));
            up2 = second3 / 10;
            up3 = second3 % 10;
            t = 10;
            while (ok == 5 && BACK)
            {
                wrcomm(programme[t]);
                if (!UP)
                {
                    MSDELAY(300);
                    up1 = 9;
                    t++;
                    if (t > 11)
                    {
                        t = 10;
                    }
                }
                if (!DOWN)
                {
                    MSDELAY(300);
                    up1++;
                    if (up1 > 9)
                    {
                        up1 = 0;
                    }
                    wrdata(up1 + 48);
                    if (t == 10)
                    {
                        up2 = up1;
                    }
                    if (t == 11)
                    {
                        up3 = up1;
                    }
                }
                if (!OK)
                {
                    MSDELAY(300);
                    ok++;
                }
                if (!BACK)
                {
                    ok = 4;
                    MSDELAY(300);
                }
            }
            second3 = ((up2 * 10) + (up3 * 1));
            up2 = minute4 / 10;
            up3 = minute4 % 10;
            t = 23;
            while (ok == 6 && BACK)
            {
                wrcomm(programme[t]);
                if (!UP)
                {
                    MSDELAY(300);
                    up1 = 9;
                    t++;
                    if (t > 24)
                    {
                        t = 23;
                    }
                }
                if (!DOWN)
                {
                    MSDELAY(300);
                    up1++;
                    if (up1 > 9)
                    {
                        up1 = 0;
                    }
                    wrdata(up1 + 48);
                    if (t == 23)
                    {
                        up2 = up1;
                    }
                    if (t == 24)
                    {
                        up3 = up1;
                    }
                }
                if (!OK)
                {
                    MSDELAY(300);
                    ok++;
                }
                if (!BACK)
                {
                    ok = 5;
                    MSDELAY(300);
                }
            }
            minute4 = ((up2 * 10) + (up3 * 1));
            up2 = second4 / 10;
            up3 = second4 % 10;
            t = 27;
            while (ok == 7 && BACK)
            {
                wrcomm(programme[t]);
                if (!UP)
                {
                    MSDELAY(300);
                    up1 = 9;
                    t++;
                    if (t > 28)
                    {
                        t = 27;
                    }
                }
                if (!DOWN)
                {
                    MSDELAY(300);
                    up1++;
                    if (up1 > 9)
                    {
                        up1 = 0;
                    }
                    wrdata(up1 + 48);
                    if (t == 27)
                    {
                        up2 = up1;
                    }
                    if (t == 28)
                    {
                        up3 = up1;
                    }
                }
                if (!OK)
                {
                    MSDELAY(300);
                    ok++;
                }
                if (!BACK)
                {
                    MSDELAY(300);
                    ok = 6;
                }
            }
            second4 = ((up2 * 10) + (up3 * 1));
        }
        if (step == 0)
        {
            write_eeprom(sec1[s], second1);
            write_eeprom(sec2[s], second2);
            write_eeprom(wait1[s], minute1);
            write_eeprom(shake1[s], minute2);
            write_eeprom(sec3[s], second3);
            write_eeprom(sec4[s], second4);
            write_eeprom(wait2[s], minute3);
            write_eeprom(shake2[s], minute4);
        }
        if (step == 1)
        {
            write_eeprom(sec5[s], second1);
            write_eeprom(sec6[s], second2);
            write_eeprom(wait3[s], minute1);
            write_eeprom(shake3[s], minute2);
            write_eeprom(sec7[s], second3);
            write_eeprom(sec8[s], second4);
            write_eeprom(wait4[s], minute3);
            write_eeprom(shake4[s], minute4);
        }
        if (step == 2)
        {
            write_eeprom(sec9[s], second1);
            write_eeprom(sec10[s], second2);
            write_eeprom(wait5[s], minute1);
            write_eeprom(shake5[s], minute2);
            write_eeprom(sec11[s], second3);
            write_eeprom(sec12[s], second4);
            write_eeprom(wait6[s], minute3);
            write_eeprom(shake6[s], minute4);
        }
        if (step == 3)
        {
            write_eeprom(sec13[s], second1);
            write_eeprom(sec14[s], second2);
            write_eeprom(wait7[s], minute1);
            write_eeprom(shake7[s], minute2);
            write_eeprom(sec15[s], second3);
            write_eeprom(sec16[s], second4);
            write_eeprom(wait8[s], minute3);
            write_eeprom(shake8[s], minute4);
        }
        if (step == 4)
        {
            write_eeprom(sec17[s], second1);
            write_eeprom(sec18[s], second2);
            write_eeprom(wait9[s], minute1);
            write_eeprom(shake9[s], minute2);
            write_eeprom(sec19[s], second3);
            write_eeprom(sec20[s], second4);
            write_eeprom(wait10[s], minute3);
            write_eeprom(shake10[s], minute4);
        }
        ok = 0;
    }
    back = 3;
}
// 0000000000000000000000000000000000000000000000000000000000000000000000000000
// Function: program
// THIS IS for what value we are entering depends on it will take a program number
// 0000000000000000000000000000000000000000000000000000000000000000000000000000

program()
{
    MSDELAY(1000);
    wrcomm(0x01);
    wrcomm(0x80);
    LCD_Display(" PROGRAM NO:  ");
    wrcomm(0x8c);
    while (OK && BACK)
    {
        if (!UP)
        {
            MSDELAY(700);
            s++;
            if (s > 4)
            {
                s = 0;
            }
            z1 = s / 10;
            z2 = s % 10;
            wrcomm(0x8c);
            wrdata(z1 + 48);
            wrdata(z2 + 48);
        }
    }
    step = 0;
    back = 2;
    data = 1;
    if (!BACK)
    {
        data = 0;
        back = 0;
        line = 0;
        SWDTEN = 1;
    }
    MSDELAY(1000);
}

// 0000000000000000000000000000000000000000000000000000000000000000000000000000
// Function: temperature degree
// THIS IS the value to enter a temperature in each program
// 0000000000000000000000000000000000000000000000000000000000000000000000000000

degree()
{
    MSDELAY(500);
    temp = read_eeprom(temper[s]);
    wrcomm(0x01);
    wrcomm(0x80);
    LCD_Display(" TEMPERATURE:");
    up2 = temp / 10;
    up3 = temp % 10;
    wrcomm(0x8d);
    wrdata(up2 + 48);
    wrdata(up3 + 48);
    wrdata('C');
    t = 13;
    while (OK && BACK)
    {
        wrcomm(programme[t]);
        if (!UP)
        {
            MSDELAY(500);
            up1 = 9;
            t++;
            if (t > 14)
            {
                t = 13;
            }
        }
        if (!DOWN)
        {
            MSDELAY(500);
            up1++;
            if (up1 > 9)
            {
                up1 = 0;
            }
            wrdata(up1 + 48);
            if (t == 13)
            {
                up2 = up1;
            }
            if (t == 14)
            {
                up3 = up1;
            }
        }
        temp = ((up2 * 10) + (up3 * 1));
        write_eeprom(temper[s], temp);
    }
    back = 4;
    MSDELAY(1000);
}

// 0000000000000000000000000000000000000000000000000000000000000000000000000000
// Function: temperature degree
// THIS IS the value to enter a drytime in each program
// 0000000000000000000000000000000000000000000000000000000000000000000000000000

dryingtime()
{
    MSDELAY(500);
    temp_time = read_eeprom(drytime[s]);
    wrcomm(0x01);
    wrcomm(0x80);
    LCD_Display("   DRY-TIME:");
    up2 = temp_time / 100;
    up3 = (temp_time / 10) % 10;
    up4 = temp_time % 10;
    wrcomm(0x8c);
    wrdata(up2 + 48);
    wrdata(up3 + 48);
    wrdata(up4 + 48);
    wrdata('s');
    t = 12;
    while (OK && BACK)
    {
        wrcomm(programme[t]);
        if (!UP)
        {
            MSDELAY(500);
            up1 = 9;
            t++;
            if (t > 14)
            {
                t = 12;
            }
        }
        if (!DOWN)
        {
            MSDELAY(500);
            up1++;
            if (up1 > 9)
            {
                up1 = 0;
            }
            wrdata(up1 + 48);
            if (t == 12)
            {
                up2 = up1;
            }
            if (t == 13)
            {
                up3 = up1;
            }
            if (t == 14)
            {
                up4 = up1;
            }
        }
        temp_time = ((up2 * 100) + (up3 * 10) + (up4 * 1));
        write_eeprom(drytime[s], temp_time);
    }
    line = 0;
    MSDELAY(500);
}

void main()
{
    MSDELAY(1000);
    ADCON1 = 0X0E;
    TRISA = 0XFF;
    TRISBbits.RB0 = 1;
    TRISBbits.RB1 = 1;
    TRISBbits.RB2 = 1;
    TRISBbits.RB3 = 1;
    TRISBbits.RB7 = 0;
    TRISBbits.RB6 = 0;
    TRISBbits.RB5 = 0;
    TRISC = 0x00;
    TRISD = 0X00;
    TRISE = 0X00;
    LCD_intlz();
    HEATER = 0;
    wrcomm(0x80);
    LCD_Display("INITIALISING");
    EN_STEPPER = 1;
    EN_STEPPER1 = 0;
    home_pos();
    EN_STEPPER1 = 1;
    EN_STEPPER = 0;
    dc_homepos();
    delay_ms(20000);

    while (1)
    {
        EN_STEPPER = 1;
        EN_STEPPER1 = 1;
        wrcomm(0x01);
        wrcomm(0x80);
        LCD_Display(" RUN OLD PROGRAM");
        wrcomm(0xc0);
        LCD_Display(" ENTER NEW PROGRAM");
        wrcomm(0x0f);
        wrcomm(0x80);
        back = 1;
        while (OK)
        {
            if (!UP)
            {
                line = 1;
                delay_ms(500);
                wrcomm(0x80);
            }
            if (!DOWN)
            {
                line = 2;
                delay_ms(500);
                wrcomm(0xc0);
            }
        }

        // To select RUN OLD PROGRAM or ENTER NEW PROGRAM
        //---------------------------------------------------------------------------
        if (line == 1)
        {
            msdelay(500);
            program();
            if (data == 1)
            {
                get_parameter(); // read values from eeprom and starts process
                hun4 = temp_time / 100;
                ten4 = (temp_time / 10) % 10;
                one4 = temp_time % 10;
                EN_STEPPER1 = 0;
                DIR1 = HIGH;
                msdelay(1000);
                motor1(3500, speed);
                dry(temp_time * 0.923); // THIS FACTOR IS MULTIPIED HERE TO MAKE ACCURATE TIME DELAY
                // end of process reset controller
                SWDTEN = 1;
            }
        }
        while (line == 2)
        {
            if (back == 1)
            {
                program();
            }
            if (back == 2)
            {
                set_parameter(); // set new  values to eeprom
            }
            if (back == 3)
            {
                degree();
            }
            if (back == 4)
            {
                dryingtime();
            }
            SWDTEN = 1;
        }
    }
}
