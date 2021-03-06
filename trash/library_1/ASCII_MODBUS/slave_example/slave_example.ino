// This code demonstrates how to use the MODBUS slave functions

// AVR GCC libraries for more information see:
//     http://www.nongnu.org/avr-libc/user-manual/modules.html
//     https://www.gnu.org/software/libc/manual/

    #include <avr/io.h>
    #include <avr/interrupt.h>
    #include <stdint.h>
    #include <string.h>
    #include <ctype.h>

// Arduino libraries: see http://arduino.cc/en/Reference/Libraries

    #include <SoftwareSerial.h>

// Project specific includes

    #include "configuration.h"
    #include "error.h"
    #include "USART.h"
    


#define RS_485_DIR_PIN              2

#define USART_TIMEOUT               100

#define read_registers              3
#define write_single_register       6
#define write_multiple_registers    16

#define my_address                  0x01                            // TODO make this a int retrieved from EEPROM or from dip switch


#include "USART.h"
#include "ASCII_MODBUS.h"

#define BB_serial_max_char      50

SoftwareSerial BB_serial(10, 11);                                   // RX, TX for bit bang serial

void setup(){

    BB_serial.begin(9600);
    MODBUS_init(RS_485_DIR_PIN, USART_TIMEOUT);
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

    char line[BB_serial_max_char];

    uint16_t MODBUS_addr;
    uint16_t n;
    uint16_t MODBUS_data;
    uint16_t i;
    uint16_t word;

    static uint8_t waiting = 0;
    
    if (!waiting){
        snprintf(line, BB_serial_max_char, "Waiting for a MODBUS string\n");
        BB_serial.print(line);
        waiting = 1;
    }
/*
    if (MODBUS_slave_is_new_msg()){

        if (MODBUS_is_for_me(my_address)){

            switch(MODBUS_get_function_code()){

                case read_registers:

                    MODBUS_addr = MODBUS_get_addr();
                    MODBUS_n = MODBUS_number_of_reg_to_read();

                    for(i = 0; i < n; i++){

                        // FIXME insert your getters here

                         switch(MODBUS_addr)

                             case test:

                                break;

                        MODBUS_slave_collect_registers(i, word);
                    }

                    MODBUS_slave_put_N_words();

                    break;

                case write_single_register:

                    MODBUS_addr = MODBUS_get_reg_to_write();
                    MODBUS_data = MODBUS_get_data_to_write();

                    //FIXME insert your setters here

                    MODBUS_echo( );                                     // when writing a single register simple echo the original string

                    break;

                case write_multiple_registers:

                    //FIXME develop these function when you have time


                    break;

                case default;

                //FIXME debug output error in
 
            }// end switch
        }// end if (MODBUS_is_for_me()){
    }// end if (MODBUS_retrieve_new_msg()){
      
      */
}// end loop(){
