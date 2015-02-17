
    #include <avr/io.h>
    #include <avr/interrupt.h>
    #include <stdint.h>
    #include <string.h>
    #include <ctype.h>
    #include <Arduino.h>

    #include "GS1_support.h"
    #include "ASCII_MODBUS.h"



/**
 * @brief Simplified function used to set the rotational velocity of the GS1 drive.
 *
 * @param physical_addr a byte identifying a particular GS1 device.  Note this
 *        must be manually programmed into the GS1.
 *
 * @param deci_freq is a 16-bit value used to set the actual motor synchronous speed.
 *        For example, if a 40 Hz operation is desired then set deci_freq = 400.
 *
 * @return result of operation, 1 = success, 0 = failure
 *
 */
 uint8_t GS1_set_speed(uint8_t physical_addr, uint16_t deci_freq){

    return MODBUS_put_word(physical_addr, Serial_Comm_Speed_Reference, deci_freq);
}




/**
 * @brief Simplified function to activate a particular GS1 motor drive.
 *
 * @param physical_addr a byte identifying a particular GS1 device.  Note this
 *        must be manually programmed into the GS1.
 *
 * @return result of operation, 1 = success, 0 = failure
 *
 */
    uint8_t GS1_turn_on(uint8_t physical_addr){

        return MODBUS_put_word(physical_addr, Serial_Comm_RUN_Command, 0x0001);
    }




/**
 * @brief Simplified function to secure a particular GS1 motor drive.
 *
 * @param physical_addr a byte identifying a particular GS1 device.  Note this
 *        must be manually programmed into the GS1.
 *
 * @return result of operation, 1 = success, 0 = failure
 *
 */
    uint8_t GS1_turn_off(uint8_t physical_addr){

        return MODBUS_put_word(physical_addr, Serial_Comm_RUN_Command, 0x0000);

    }
