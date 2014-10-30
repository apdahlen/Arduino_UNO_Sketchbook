/*
 * ADC test.
 *
 * Copyright 2014 Aaron P. Dahlen       APDahlen@gmail.com
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
    #include "USART.h"
    #include "adc.h"

// Global variables

    char line[BUF_LEN];

    LiquidCrystal lcd (LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup(){

    ADC_init();
    USART_init(F_CLK, BAUD_RATE);                   // The USART code must be placed in your Arduino sketchbook
    USART_set_terminator(LINE_TERMINATOR);

    sprintf(line, "ADC test\n");
    USART_puts(line);

    lcd.begin(16, 2);                               // Define LCD as a 2-line by 16 char device
    lcd.setCursor(0, 0);                            // Point to the LCD line 1 upper right character position
    lcd.print("ADC test");
    lcd.setCursor(0, 1);                            // Point to LCD line 2 left character position
    lcd.print("Edit: 30 Oct 14");

    tone(BUZ_PIN, 554);    // C                     // Make a happy noise
    delay(200);
    tone(BUZ_PIN, 659);    // E
    delay(100);
    tone(BUZ_PIN, 784);    // G
    delay(100);
    tone(BUZ_PIN, 1047);   // C
    delay(200);
    noTone(BUZ_PIN);

    delay(1000);

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

    int intensity;

    int joy_vert, joy_horz;

    static uint32_t next_LCD_update_time = 0;       // Think of the Arduino loop as a function.
                                                    // Use static to remember between loops...

    uint32_t current_time;

    uint16_t ADC_results[4];

    current_time = millis();

    if(current_time >= next_LCD_update_time){

        next_LCD_update_time = current_time + 200;  // FIXME no magic number

        digitalWrite(13, HIGH);                     // This is a the ADC test - use scope to see time
        consecutive_ADC_read(ADC_results, 4);
        digitalWrite(13, LOW);

        joy_vert = ADC_results[0];
        joy_horz = ADC_results[1];

        joy_vert = -joy_vert + 512;                 // remove offset so "home" position reads close to zero 
        joy_horz -= 512;

        snprintf(line, 17, "V =%4d, H =%4d", joy_vert, joy_horz);
        lcd.setCursor(0, 0);
        lcd.print(line);
    }
}
