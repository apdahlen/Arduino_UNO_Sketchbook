/*
 * Test of the Automation Direct GS1 Variable Frequency Drive
 *
 * Copyright 2015 Aaron P. Dahlen       APDahlen@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */


// AVR GCC libraries for more information see:
//     http://www.nongnu.org/avr-libc/user-manual/modules.html
//     https://www.gnu.org/software/libc/manual/

    #include <avr/io.h>
    #include <avr/interrupt.h>
    #include <stdint.h>
    #include <string.h>
    #include <ctype.h>


// Arduino libraries: see http://arduino.cc/en/Reference/Libraries

    #include <LiquidCrystal.h>


// Project specific includes

    #include "configuration.h"
  // #include "GS1_USART.h"
   // #include "line_parser.h"
    #include "ASCII_MODBUS.h"
    #include "GS1_support.h"
    #include "USART.h"
#include "error.h"


// Global variables

    char line[BUF_LEN];
    LiquidCrystal lcd (LCD_RS,  LCD_E, LCD_D4,  LCD_D5, LCD_D6, LCD_D7);   // Yes, this is a variable!


void setup(){

//    digitalWrite(RS_485_DIR_PIN, LOW);
//    pinMode(RS_485_DIR_PIN, OUTPUT);

    GS1_init(GS1_ADDR, RS_485_DIR_PIN, MODBUS_TIMEOUT);

    lcd.begin(16, 2);                               // Define LCD as a 2-line by 16 char device
    lcd.setCursor(0, 0);                            // Point to the LCD line 1 upper right character position
    lcd.print("Test of GS1");
    lcd.setCursor(0, 1);                            // Point to LCD line 2 left character position
    lcd.print("Edit: 21 Mar 15");

    lcd.clear();
}



/*********************************************************************************
 *  ______  ____   _____   ______  _____  _____    ____   _    _  _   _  _____
 * |  ____|/ __ \ |  __ \ |  ____|/ ____||  __ \  / __ \ | |  | || \ | ||  __ \
 * | |__  | |  | || |__) || |__  | |  __ | |__) || |  | || |  | ||  \| || |  | |
 * |  __| | |  | ||  _  / |  __| | | |_ ||  _  / | |  | || |  | || . ` || |  | |
 * | |    | |__| || | \ \ | |____| |__| || | \ \ | |__| || |__| || |\  || |__| |
 * |_|     \____/ |_|  \_\|______|\_____||_|  \_\ \____/  \____/ |_| \_||_____/
 *
 ********************************************************************************/

ISR(USART_RX_vect){

 /**
 * @note This Interrupt Service Routine is called when a new character is received by the USART.
 * Ideally it would have been placed in the USART.cpp file but there is a error "multiple definition
 * of vector_18".  Apparently Arduino detects when an ISR is in the main sketch.  If you place it
 * somewhere else it is missed and replaced with the Arduino handler.  This is the source of the
 * multiple definitions error -  * see discussion @ http://forum.arduino.cc/index.php?topic=42153.0
 */
    USART_handle_ISR();
}




/*********************************************************************************
 *  ____            _____  _  __ _____  _____    ____   _    _  _   _  _____
 * |  _ \    /\    / ____|| |/ // ____||  __ \  / __ \ | |  | || \ | ||  __ \
 * | |_) |  /  \  | |     | ' /| |  __ | |__) || |  | || |  | ||  \| || |  | |
 * |  _ <  / /\ \ | |     |  < | | |_ ||  _  / | |  | || |  | || . ` || |  | |
 * | |_) |/ ____ \| |____ | . \| |__| || | \ \ | |__| || |__| || |\  || |__| |
 * |____//_/    \_\\_____||_|\_\\_____||_|  \_\ \____/  \____/ |_| \_||_____/
 *
 ********************************************************************************/

void loop(){

 //   uint8_t test_str_hex[] =  {0x01, 0x10, 0x09, 0x1B, 0x00, 0x02, 0x04, 0x02, 0x58, 0x00, 0x01};

 //   uint8_t test_str_hex[] =  {0x01, 0x03, 0x04, 0x01, 0x00, 0x01}; 
  //  uint8_t length_test_str_hex = 6;

  //  uint8_t run_str_hex[] = { GS1_dev_adr, GS1_write, 0x09, 0x1B, 0x00, 0x01} ;     // Works!
 //  uint8_t stop_str_hex[] = { GS1_dev_adr, GS1_write, 0x09, 0x1B, 0x00, 0x00} ;     // works!

 //   uint8_t speed_str_hex[] = { GS1_dev_adr, GS1_write, 0x09, 0x1A, 0x00, 0xA0} ;   // OK


    while(1){

        GS1_turn_on(GS1_ADDR);
        delay(1000);

        for(uint16_t i = 50; i < 600; i++){
            GS1_set_speed(GS1_ADDR, i);
            delay(5);
        }

        delay(4000);
        GS1_turn_off(GS1_ADDR);

       delay(30000);

    }
}

    //    LRC = LRC_gen(test_str_hex, length_test_str_hex);

    //  byte_array_2_str(line, length_test_str_hex, test_str_hex);

    //   pack_ASCII_str(line, test_str_hex, length_test_str_hex);
    //     pack_ASCII_str(line, stop_str_hex, 6);


    //   digitalWrite(RS_485_DIR_PIN, WRITE);
    //   USART_puts(line);
    //   delayMicroseconds(1500);                 // There is a glitch as the RS-485 tranceiver transitions from XMT to RCV.  This 
                                                 // delay moves the transition outside of the GS1's observation window.  Note that
                                                 // the GS1 takes approximatly 2.5 mS to start a reply. 
    //   digitalWrite(RS_485_DIR_PIN, READ);
    //      while(1)

    //     uint16_t CRC =  GS1_CRC_gen(test_str_hex, length_test_str_hex);

    //   sprintf(line, "%u\n", CRC);

    // USART_puts(line);

