// This code demonstrates how to use the MODBUS slave functions

#define RS_485_DIR_PIN              2

#define USART_TIMEOUT               100

#define read_registers              3
#define write_single_register       6
#define write_multiple_registers    16

#define my_address                  0x01                            // TODO make this a int retrieved from EEPROM or from dip switch

#include <SoftwareSerial.h>

#define BB_serial_max_char      50

SoftwareSerial BB_serial(10, 11);                                   // RX, TX for bit bang serial

void setup(){

    BB_serial.begin(9600);
    MODBUS_init(RS_485_DIR_PIN, USART_TIMEOUT);
    

}

loop(){

    char line[BB_serial_max_char];

    uint16_t MODBUS_addr;
    uint16_t n;
    uint16_t MODBUS_data;
    uint16_t i;
    uint16_t word;

    uint8_t waiting = 0;


    if (!waiting){
        snprintf(line, BB_serial_max_char, "Waiting for a MODBUS string\n");
        BB_serial.prtint(line);
        waiting = 1;
    }

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
}// end loop(){
