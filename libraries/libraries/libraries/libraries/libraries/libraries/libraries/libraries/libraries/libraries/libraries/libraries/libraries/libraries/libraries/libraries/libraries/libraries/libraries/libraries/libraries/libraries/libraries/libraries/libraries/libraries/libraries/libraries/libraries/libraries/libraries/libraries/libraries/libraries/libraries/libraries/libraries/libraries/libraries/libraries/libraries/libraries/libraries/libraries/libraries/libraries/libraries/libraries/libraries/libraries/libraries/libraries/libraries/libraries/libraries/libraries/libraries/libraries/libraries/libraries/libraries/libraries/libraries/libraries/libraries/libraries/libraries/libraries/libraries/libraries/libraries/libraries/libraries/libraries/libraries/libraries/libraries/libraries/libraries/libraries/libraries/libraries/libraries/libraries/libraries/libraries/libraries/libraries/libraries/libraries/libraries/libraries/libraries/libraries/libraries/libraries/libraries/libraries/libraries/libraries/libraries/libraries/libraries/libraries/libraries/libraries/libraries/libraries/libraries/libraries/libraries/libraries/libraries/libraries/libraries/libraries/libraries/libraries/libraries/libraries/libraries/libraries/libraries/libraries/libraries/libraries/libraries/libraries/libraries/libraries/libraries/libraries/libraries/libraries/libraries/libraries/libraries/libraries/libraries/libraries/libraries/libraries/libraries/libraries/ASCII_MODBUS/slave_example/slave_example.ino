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

    #ifdef DEBUG

        #include <SoftwareSerial.h>

    #endif


// Project specific includes

    #include "configuration.h"
    #include "error.h"
    #include "USART.h"
    #include "ASCII_MODBUS.h"


// Global variables

    #ifdef DEBUG

        SoftwareSerial BB_serial(10, 11);                                   // RX, TX for bit bang serial
        char line[BB_serial_max_char];

    #endif

    uint16_t addr;
    uint16_t n;
    uint16_t data;
    uint16_t i;
    uint16_t code;

// Function declarations

    void service_read_holding_reg(void);
    void service_preset_single_reg(void);
    void service_preset_multiple_regs(void);


void setup(){

    #ifdef DEBUG

        BB_serial.begin(9600);
        MODBUS_init(RS_485_DIR_PIN, USART_TIMEOUT);

    #endif

}

/***************************************************************************************************
 *  ______  ____   _____   ______  _____  _____    ____   _    _  _   _  _____
 * |  ____|/ __ \ |  __ \ |  ____|/ ____||  __ \  / __ \ | |  | || \ | ||  __ \
 * | |__  | |  | || |__) || |__  | |  __ | |__) || |  | || |  | ||  \| || |  | |
 * |  __| | |  | ||  _  / |  __| | | |_ ||  _  / | |  | || |  | || . ` || |  | |
 * | |    | |__| || | \ \ | |____| |__| || | \ \ | |__| || |__| || |\  || |__| |
 * |_|     \____/ |_|  \_\|______|\_____||_|  \_\ \____/  \____/ |_| \_||_____/
 *
 **************************************************************************************************/

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

/***************************************************************************************************
 *  ____            _____  _  __ _____  _____    ____   _    _  _   _  _____
 * |  _ \    /\    / ____|| |/ // ____||  __ \  / __ \ | |  | || \ | ||  __ \
 * | |_) |  /  \  | |     | ' /| |  __ | |__) || |  | || |  | ||  \| || |  | |
 * |  _ <  / /\ \ | |     |  < | | |_ ||  _  / | |  | || |  | || . ` || |  | |
 * | |_) |/ ____ \| |____ | . \| |__| || | \ \ | |__| || |__| || |\  || |__| |
 * |____//_/    \_\\_____||_|\_\\_____||_|  \_\ \____/  \____/ |_| \_||_____/
 *
 **************************************************************************************************/


void loop(){

    #ifdef DEBUG

        static uint8_t waiting = 0;

        if(!waiting){
            snprintf(line, BB_serial_max_char, "Waiting for a MODBUS string\n");
            BB_serial.print(line);
            waiting = 1;
        }

    #endif

    if(MODBUS_slave_is_new_msg()){

        #ifdef DEBUG

            snprintf(line, BB_serial_max_char, "New message received!\n");
            BB_serial.print(line);
            waiting = 0;

        #endif

        #define MODBUS_SLAVE_ADDR           1       // Specify the starting positions for the various fields
        #define MODBUS_FUNCTION             3

        if(MODBUS_get_Nth_int(MODBUS_SLAVE_ADDR) == MY_ADDR){

            code = MODBUS_get_Nth_int(MODBUS_FUNCTION);

            #ifdef DEBUG

                snprintf(line, BB_serial_max_char, "\t and its for me\n");
                BB_serial.print(line);
                snprintf(line, BB_serial_max_char, "\t function code = %x\n", code);
                BB_serial.print(line);

            #endif

            switch(code){

                case READ_HOLDING_REGISTERS:
                    service_read_holding_reg();
                    break;

                case PRESET_SINGLE_REGISTER:
                    service_preset_single_reg();
                    break;

                case PRESET_MULTIPLE_REGISTERS:
                    service_preset_multiple_regs();
                    break;

                default:
                    ;
            }// end switch

        }// end if (MODBUS_is_for_me()){
    }// end if (MODBUS_retrieve_new_msg()){
}// end loop(){




/** ************************************************************************************************
 *
 *  MODBUS mode 3: read holding registers
 *
 *     ----------------------------------------------------
 *    | Master Frame Format                                |
 *    |----------------------------------------------------|
 *    | Name                     | Length (char) | Example |
 *    |--------------------------|---------------|---------|
 *    | Start                    |      1        |  :      |
 *    | Slave address            |      2        |  01     |
 *    | Function                 |      2        |  03     |
 *    | Starting register addr   |      4        |  019B   |
 *    | Number of words to read  |      4        |  0001   |
 *    | LRC                      |      2        |  FIXME  |
 *    | (CR/LF) pair             |      2        |         |
 *     ----------------------------------------------------
 *
 * @note This routine contains two predefined tests.
 *
 *  1) Reading address 0x0000 with N = 1 will return 0XAAAA
 *
 *  2) Reading address 0x0001 will a request for n words returns the requested n words.  The words
 *     contains sequential integers starting with 1.
 */

    #define MODE_3_START_DATA_ADDR      5
    #define MODE_3_NUM_WORDS_TO_READ    9

void service_read_holding_reg(void){

    addr = MODBUS_get_Nth_word(MODE_3_START_DATA_ADDR);
    n = MODBUS_get_Nth_word(MODE_3_NUM_WORDS_TO_READ);

    #ifdef DEBUG

        snprintf(line, BB_serial_max_char, "\t starting addr = %x\n", addr);
        BB_serial.print(line);
        snprintf(line, BB_serial_max_char, "\t num regs to read = %x\n", n);
        BB_serial.print(line);

    #endif

    switch(addr){

        case 0x0000:                                                // hardcode a test case: reads from address 0 return "AAAA"

            MODBUS_buffer_words(0, 0xABCD);
            MODBUS_put_N_words(1, MY_ADDR);

            break;

        case 0x0001:                                                // another test case: reads from address 1 return N words counting up from 1

            for(i = 0; i < n; i++){
                MODBUS_buffer_words(i, i + 1);
            }
            MODBUS_put_N_words(n, MY_ADDR);

            break;



        // TODO insert your cases here



        default:
            ;
 
    }// end switch
}

/** ************************************************************************************************
 *
 *  MODBUS mode 6: Preset a single register
 *
 *     ----------------------------------------------------
 *    | Master Frame Format                                |
 *    |----------------------------------------------------|
 *    | Name                     | Length (char) | Example |
 *    |--------------------------|---------------|---------|
 *    | Start                    |      1        |  :      |
 *    | Slave address            |      2        |  01     |
 *    | Function                 |      2        |  06     |
 *    | Register address         |      4        |  019B   |
 *    | Preset data              |      4        |  0001   |
 *    | LRC                      |      2        |  FIXME  |
 *    | (CR/LF) pair             |      2        |         |
 *     ----------------------------------------------------
 *
 */

    #define MODE_6_STARTING_ADDR        5
    #define MODE_6_FIRST_DATA           9

void service_preset_single_reg(void){

    addr = MODBUS_get_Nth_word(MODE_6_STARTING_ADDR);
    data = MODBUS_get_Nth_word(MODE_6_FIRST_DATA);

    #ifdef DEBUG

        snprintf(line, BB_serial_max_char, "\t starting addr = %x\n", addr);
        BB_serial.print(line);
        snprintf(line, BB_serial_max_char, "\t data = %x\n", data);
        BB_serial.print(line);

    #endif

    //FIXME insert your setter here

    MODBUS_slave_echo( );                                     // when writing a single register simple echo the original string
    
    #ifdef DEBUG

        snprintf(line, BB_serial_max_char, "%s", MODBUS_cmd_line);
        BB_serial.print(line);

    #endif

}



/** ************************************************************************************************
 *
 *  MODBUS mode 16: Preset a multiple register
 *
 *     ----------------------------------------------------
 *    | Master Frame Format                                |
 *    |----------------------------------------------------|
 *    | Name                     | Length (char) | Example |
 *    |--------------------------|---------------|---------|
 *    | Start                    |      1        |  :      |
 *    | Slave address            |      2        |  01     |
 *    | Function                 |      2        |  16     |
 *    | Starting address         |      4        |  019B   |
 *    | Num registers            |      2        |  0001   |
 *    | Data                     |      N        |  N      |
 *    | LRC                      |      2        |  FIXME  |
 *    | (CR/LF) pair             |      2        |         |
 *     ----------------------------------------------------
 *
 */

    #define MODE_16_STARTING_ADDR        5
    #define MODE_16_BYTE_COUNT           9
    #define MODE_16_FIRST_DATA           11

void service_preset_multiple_regs(void){

    //TODO develop these function when you have time

    addr = MODBUS_get_Nth_word(MODE_6_STARTING_ADDR);
    n = MODBUS_get_Nth_int(MODE_16_BYTE_COUNT);


    switch(addr){

        case 0x0000:                        // typical

            for(i = 0; i < n; i++){

                data = MODBUS_get_Nth_word(MODE_6_FIRST_DATA + i);

                //FIXME Insert your setter here

                //MODBUS_buffer_words(i, i + 1);    // TODO need to make the return string
            }
            //MODBUS_put_N_words(n, MY_ADDR);

        break;

        default:
            ;

    }// end switch
}
