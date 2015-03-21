
#ifndef configuration_H
    #define configuration_H

    #define F_CLK 16000000UL
    #define BAUD_RATE 19200L

    #define LINE_TERMINATOR 0x0A    // ASCII Line Feed

    #define BUF_LEN 100

    #define MAX_FIELDS 20           // must be less than 127

    // Configure Liquid Crystal Display

        #define LCD_RS 8
        #define LCD_E 7
        #define LCD_D4 5            // It would have been nice to use the pro-mini A6 and A7 pins
        #define LCD_D5 4            // with the LCD.  Unfortunately, these are I/O are designed for
        #define LCD_D6 A4           // analog input only.
        #define LCD_D7 A5

    #define LED_PIN 13
    #define BUZ_PIN 2

    #define GS1_ADDR 0x01
    #define RS_485_DIR_PIN 10
    #define MODBUS_TIMEOUT 1000


#endif
