
#ifndef configuration_H
    #define configuration_H

    #define F_CLK 16000000UL
    #define BAUD_RATE 9600L

    #define LINE_TERMINATOR 0x0A    // ASCII Line Feed

    #define BUF_LEN 100

    // Configure Liquid Crystal Display

        #define LCD_RS 8
        #define LCD_E 7
        #define LCD_D4 5            // It would have been nice to use the pro-mini A6 and A7 pins
        #define LCD_D5 4            // with the LCD.  Unfortunately, these are I/O are designed for
        #define LCD_D6 A4           // analog input only.
        #define LCD_D7 A5



    #define RS_485_DIR_PIN              2

    #define USART_TIMEOUT               100

    #define read_registers              3
    #define write_single_register       6
    #define write_multiple_registers    16

    #define MY_ADDR                  0x01                            // TODO make this an int retrieved from EEPROM or from dip switch

    #define BB_serial_max_char          50

#endif
