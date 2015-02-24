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
    #include "ASCII_MODBUS.h"

// Global variables

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

    uint16_t addr;
    uint16_t n;
    uint16_t data;
    uint16_t i;
    uint16_t code;

    static uint8_t waiting = 0;

    if(!waiting){
        snprintf(line, BB_serial_max_char, "Waiting for a MODBUS string\n");
        BB_serial.print(line);
        waiting = 1;
    }

    if(MODBUS_slave_is_new_msg()){
        snprintf(line, BB_serial_max_char, "New message received!\n");
        BB_serial.print(line);
        waiting = 0;

#define MODBUS_PHYSICAL_ADDR        1       // Specify the starting positions for the various fields
#define MODBUS_CMD_CODE             3

#define MODE_3_START_DATA_ADDR      5
#define MODE_3_NUM_WORDS_TO_READ    9


#define MODE_6_START_ADDR           5
#define MODE_6_FIRST_DATA           9

          if(MODBUS_get_Nth_int(MODBUS_PHYSICAL_ADDR) == MY_ADDR){
            //snprintf(line, BB_serial_max_char, "\t and its for me\n");
            //BB_serial.print(line);

            code = MODBUS_get_Nth_int(MODBUS_CMD_CODE);
                //snprintf(line, BB_serial_max_char, "\t function code = %x\n", code);
                //BB_serial.print(line);



            switch(code){

            /***************************************************************************************
             *
             *  MODBUS mode 3: read holding registers
             *
             *     ----------------------------------------------------
             *    | Master Frame Format                                |
             *    |----------------------------------------------------|
             *    | Name                     | Length (char) | Example |
             *    |--------------------------|---------------|---------|
             *    | Start                    |      1        |  :      |
             *    | Station address          |      2        |  01     |
             *    | Function                 |      2        |  03     |
             *    | Starting register addr   |      4        |  019B   |
             *    | Number of words to read  |      4        |  0001   |
             *    | LRC                      |      2        |  FIXME  |
             *    | (CR/LF) pair             |      2        |         |
             *     ----------------------------------------------------
             *
             * This slave will respond
             */

                case READ_HOLDING_REGISTERS:

                    addr = MODBUS_get_Nth_word(MODE_3_START_DATA_ADDR);
                    n = MODBUS_get_Nth_word(MODE_3_NUM_WORDS_TO_READ);


                    //snprintf(line, BB_serial_max_char, "\t starting addr = %x\n", addr);
                    //BB_serial.print(line);

                    //snprintf(line, BB_serial_max_char, "\t num regs to read = %x\n", n);
                    //BB_serial.print(line);

                    switch(addr)

                        case 0x0000:                                           // hardcode a test case: reads from address 0 return "AAAA"

                            MODBUS_buffer_words(0, 0xAAAA);
                            MODBUS_put_N_words(1, MY_ADDR);

                            break;

                        case 0x0001:                                           // another test case: reads from address 1 return N words counting up from 1

                            MODBUS_buffer_words(0, 0xAAAA);

                            for(i = 0; i < n; i++){
                                MODBUS_buffer_words(i, i + 1);
                            }
                            MODBUS_put_N_words(n, MY_ADDR);

                            break;


                        // TODO insert your cases here


                    break;


            /***************************************************************************************
             *
             *  MODBUS mode 6: Preset a single register
             *
             *     ----------------------------------------------------
             *    | Master Frame Format                                |
             *    |----------------------------------------------------|
             *    | Name                     | Length (char) | Example |
             *    |--------------------------|---------------|---------|
             *    | Start                    |      1        |  :      |
             *    | Station address          |      2        |  01     |
             *    | Function                 |      2        |  06     |
             *    | Register address         |      4        |  019B   |
             *    | Preset data              |      4        |  0001   |
             *    | LRC                      |      2        |  FIXME  |
             *    | (CR/LF) pair             |      2        |         |
             *     ----------------------------------------------------
             *
             *
             */

                case PRESET_SINGLE_REGISTER:

                    addr = MODBUS_get_Nth_word(MODE_6_START_ADDR);
                    data = MODBUS_get_Nth_word(MODE_6_FIRST_DATA);

                    //snprintf(line, BB_serial_max_char, "\t starting addr = %x\n", addr);
                    //BB_serial.print(line);

                    //snprintf(line, BB_serial_max_char, "\t data = %x\n", data);
                    //BB_serial.print(line);

                    //FIXME insert your setters here

                    MODBUS_slave_echo( );                                     // when writing a single register simple echo the original string
                    snprintf(line, BB_serial_max_char, "%s", MODBUS_cmd_line);
                    BB_serial.print(line);
                    break;


                case PRESET_MULTIPLE_REGISTERS:

                    //FIXME develop these function when you have time


                    break;

        //        default:

                    // FIXME debug output error in
 
            }// end switch
        }// end if (MODBUS_is_for_me()){
    }// end if (MODBUS_retrieve_new_msg()){
}// end loop(){
