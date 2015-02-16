/**
 * @file ASCII_MODBUS_master
 * 
 * @brief This file contains the functions necessary to implement a MODBUS master node using ASCII
 *
 *
 *    --------------------------------------------------------------------------
 *    |                       Modbus ASCII frame format
 *    --------------------------------------------------------------------------
 *    | Name      | Length (char) | Function
 *    |-----------|---------------|---------------------------------------------
 *    | Start     |      1        | Starts with colon ( : ) (ASCII hex value is 0x3A)
 *    | Address   |      2        | Station address
 *    | Function  |      2        | Indicates the function codes like read coils / inputs
 *    | Data      |      n        | Data + length will be filled depending on the message type
 *    | LRC       |      2        | Checksum
 *    | End       |      2        | Carriage return – line feed (CR/LF) pair (ASCII values of 0x0D & 0x0A)
 *
 */




// AVR GCC libraries for more information see:
// http://www.nongnu.org/avr-libc/user-manual/modules.html
// https://www.gnu.org/software/libc/manual/

    #include <avr/io.h>
    #include <avr/interrupt.h>
    #include <stdint.h>
    #include <string.h>
    #include <ctype.h>
    #include <Arduino.h>

    #include <stdio.h>


// Project specific includes

    #include "MODBUS_support.h"
    #include "MODBUS_USART.h"
    #include "configuration.h"
    #include "errors.h"


// Public variables defined

    char MODBUS_cmd_line[size_of_cmd_lines] = "MODBUS_cmd_lin not set";
    char MODBUS_reply_line[size_of_cmd_lines] = "MODBUS_cmd_lin not set";


// Private functions

    void byte_array_2_str(char *line, uint8_t length, uint8_t *hex_array);
    uint16_t LRC_gen(uint8_t *data, uint8_t length);
    void pack_ASCII_str(char *line, uint8_t *c, uint8_t length);


// Private alias


// Private variables

    char digit[ ] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};


/**
 * @brief This function performs the Longitudinal Redundancy Check  (LRC).  This
 *        is used while the MODBUS is in "ASCII mode".  To quote wiki:
 *
 *            "the 8-bit two's-complement value of the sum of all bytes modulo 2^8"
 */

    uint16_t LRC_gen(uint8_t *data, uint8_t length){

        uint8_t LRC = 0;

        while(length--){
            LRC += *data++;                                         // modulo 256 addition
        }
        return 0 - LRC;                                             // 2's complement
    }



/**
 * @brief Construct a string formatted for a MODBUS device operating in ASCII mode.
 *        Preppend the ':' symbol, converting the bytes to ASCII Hex, and appending the
 *        LRC, CR plus LF.
 *
 *        An example of a properly formatted string is:
 *            { 01 10 09 1B 00 02 04 02 58 00 01 5A 66 }
 *
 * @param *line is the destination
 * 
 * @param *c is the source containing the character string
 * 
 * @param length number of characters in the input string.  This parameter is
 *        required as the input string may contain the NULL char.
 *
 * @note reference the GS1 documentation for more information.
 */

    void pack_ASCII_str(char *line, uint8_t *c, uint8_t length){

        uint8_t LRC = LRC_gen(c, length);

        *line++ = ':';

        byte_array_2_str(line, length, c);
        line+= length << 1;
        *line++ = digit[LRC >> 4];
        *line++ = digit[LRC & 0x0F];
        *line++ = 0x0D;                                             // CR
        *line++ = 0x0A;                                             // LF
        *line++ = 0x00;
    }




/**
 * @brief This is a helper function for the pack_ASCII_str function.  It converts
 *        hex digits to their equivalent ASCII characters.  For example 0x1B is
 *        converted to {1 B}
 */

    void byte_array_2_str(char *line, uint8_t length, uint8_t *hex_array){

        while(length--){
            *line = digit[*hex_array >> 4];
            line++;
            *line = digit[*hex_array & 0x0F];
            line++;
            hex_array++;
        }
        *line = 0x00;
    }


/*******************************************************************************
 *  .___  ___.      ___           _______.___________. _______ .______
 *  |   \/   |     /   \         /       |           ||   ____||   _  \
 *  |  \  /  |    /  ^  \       |   (----`---|  |----`|  |__   |  |_)  |
 *  |  |\/|  |   /  /_\  \       \   \       |  |     |   __|  |      /
 *  |  |  |  |  /  _____  \  .----)   |      |  |     |  |____ |  |\  \----.
 *  |__|  |__| /__/     \__\ |_______/       |__|     |_______|| _| `._____|
 *
 ******************************************************************************/


/**
 * @brief This function is used to write a single word to the MODBUS.  An string
 *        is 15 char long plus 2 for terminating characters e.g.,
 *
 *         : 01 06 0100 1770 71 CR LF    (spaces included to separate the fileds)
 *
 * @param physical_addr a byte identifying a particular MODBUS device.  Note this
 *        must be manually programmed into a device such as the GS1.
 *
 * @param mem_addr a 16-bit value identifying the particular MODBUS register to be written
 *
 * @param data a 16-bit value containing the data to be written to mem_addr
 *
 * @return result of operation, 1 = success, 0 = failure
 *
 * @note  There is a glitch as the RS-485 transceiver transitions from XMT to RCV.
 *        This delay moves the transition outside of the GS1's observation window.
 *        Note that the GS1 takes approximately 2.5 mS to start a reply.
 */

    uint8_t MODBUS_put_word(uint8_t physical_addr, uint16_t mem_addr, uint16_t data) {

        #define match 0x00

        uint8_t mem_addr_h = mem_addr >> 8;
        uint8_t mem_addr_l = mem_addr & 0x00FF;

        uint8_t data_h = data >> 8;
        uint8_t data_l = data & 0x00FF;

        uint8_t cmd_str_hex[] = { physical_addr, MODBUS_WRITE, mem_addr_h, mem_addr_l, data_h, data_l } ;

        uint16_t milisecond_cnt;

        pack_ASCII_str(MODBUS_cmd_line, cmd_str_hex, 6);

    // Write the word

        digitalWrite(RS_485_DIR_PIN, BUS_WRITE);
        delayMicroseconds(1000);
        USART_puts(MODBUS_cmd_line);
        delayMicroseconds(1500);
        digitalWrite(RS_485_DIR_PIN, BUS_READ);

    // Verify the word was written by analyzing the echo

        milisecond_cnt = 0;
        while(!USART_is_string( )){
            delay(1);
            if (++milisecond_cnt > USART_TIMEOUT_MILISECONDS){                                // prevent lockup if device is not connected
              //error_message("MODBUS_put_word: USART timeout");
                strncpy(ERROR_MSG, "MODBUS_put_word: USART timeout", SIZE_ERROR_MSG);
                return 0x00;
            }
        }

        USART_gets(MODBUS_reply_line);

        if(strncmp(MODBUS_cmd_line, MODBUS_reply_line, 15) == match){   // limit to the first 15 characters (no need to test the line terminators)
            return 0x01;
        }
        else{
            return 0x00;
            strncpy(ERROR_MSG, "MODBUS_put_word: improper return from device", SIZE_ERROR_MSG);
          //error_message("MODBUS_put_word: improper return from device");
        }
    }




/**
 * @brief This function is used to read registers from MODBUS
 *
 * FIXME From the GS1 documentation page 5-72:
 *
 * FIXME Example Message:
 *      Write a value of 60Hz to P9.26 and a value of 1 to P9.27 =
 *      01 10 09 1b 00 02 04 02 58 00 01 5a 66
 *      We receive a good reply = 01 10 09 1b 00 02 a3 9f
 *
 * @param destination a pointer to the location the returned values will be placed
 *
 * @param physical_addr a byte identifying a particular MODBUS device.  Note this
 *        must be manually programmed into a device such as the GS1.
 *
 * @param starting_mem_addr a 16-bit value identifying the first address to be read
 *
 * @param get_n_words identify the number of words to be read from the addressed MODBUS device
 *
 * @note  There is a glitch as the RS-485 transceiver transitions from XMT to RCV.
 *        This delay moves the transition outside of the GS1's observation window.
 *        Note that the GS1 takes approximately 2.5 mS to start a reply.
 */

   uint8_t MODBUS_read_registers(uint16_t *destination, uint8_t physical_addr, uint16_t starting_mem_addr, uint16_t get_n_words ) {

        uint8_t starting_mem_addr_h = starting_mem_addr >> 8;
        uint8_t starting_mem_addr_l = starting_mem_addr & 0x00FF;

        uint8_t get_n_words_h = get_n_words >> 8;
        uint8_t get_n_words_l = get_n_words & 0x00FF;

        uint8_t cmd_str_hex[] = { physical_addr, MODBUS_READ, starting_mem_addr_h, starting_mem_addr_l, get_n_words_h, get_n_words_l } ;

        uint16_t milisecond_cnt;

        pack_ASCII_str(MODBUS_cmd_line, cmd_str_hex, 6);

    // Send the request for N words of data

        digitalWrite(RS_485_DIR_PIN, BUS_WRITE);
        delayMicroseconds(1000);
        USART_puts(MODBUS_cmd_line);
        delayMicroseconds(1500);
        digitalWrite(RS_485_DIR_PIN, BUS_READ);

    // Verify the word was written by analyzing the echo

        milisecond_cnt = 0;
        while(!USART_is_string( )){
            delay(1);
            if (++milisecond_cnt == USART_TIMEOUT_MILLISECONDS){         // prevent lockup if device is not connected
                strncpy(ERROR_MSG, "MODBUS_read_registers: USART timeout", SIZE_ERROR_MSG);
                return 0x00;
            }
        }

        USART_gets(MODBUS_reply_line);

    // Verify first 5 ASCII characters of response are correct

        if(strncmp(MODBUS_cmd_line, MODBUS_reply_line, 5) != match){   // limit to the first 15 characters (no need to test the line terminators)
            strncpy(ERROR_MSG, "MODBUS_read_registers: first 5 characters don't match", SIZE_ERROR_MSG);
            return 0x00;
        }

    // Verify the number bytes received equals number of bytes requested

        uint16_t num_words_received = (MODBUS_reply_line[5] << 8) + MODBUS_reply_line[6];

        if (num_words_received != get_n_words){
            strncpy(ERROR_MSG, "MODBUS_read_registers: improper number words returned", SIZE_ERROR_MSG);
            return 0x00;
        }

    // Put the received words into the destination buffer

        for (uint16_t i = 0; i < get_n_words * 2; i = i + 2){

            *destination = (MODBUS_reply_line[i + 7] << 8) + MODBUS_reply_line[i + 8];
            destination++;
        }
        return 0x01;

}


/*******************************************************************************
 *
 *       _______. __          ___   ____    ____  _______
 *      /       ||  |        /   \  \   \  /   / |   ____|
 *     |   (----`|  |       /  ^  \  \   \/   /  |  |__
 *      \   \    |  |      /  /_\  \  \      /   |   __|
 *  .----)   |   |  `----./  _____  \  \    /    |  |____
 *  |_______/    |_______/__/     \__\  \__/     |_______|
 *
 ******************************************************************************/




    static char MODBUS_str[MODBUS_STR_LENGTH];
    static uint8_t num_char;


/**
* @brief Convert an ASCII codex hex character into its integer equivalent
*
* @param a hex character in ASCII form
*
* @return an unsigned integer
*/
    uint8_t ASCII_hex_2_bin(char c) {
        //if (c = ’0’)
        //    return 0;
        if (c <= ’9’)
            return c - ’0’;
    //   if (c < ’A’)
    //       return 0;
        if (c <= ’F’)
            return (c - ’A’) + 10;
    }









 /**
  * @brief Determine if a valid MODBUS message has been received.  On successful
  *        retrieval the message is held in a local MODBUS_str[] buffer.
  *
  * @return 0 = no new message, 1 = a valid message had been retrieved
  */
    uint8_t MODBUS_retrieve_new_msg(void){

        if (!USART_is_string())
            return 0x00;

        num_char =  USART_gets(MODBUS_str);

        // FIXME add code to verify the LRC
        /
        return 0x01;

    }




 /**
  * @brief Determine if the MODBUS message held in the  MODBUS_str[] buffer
  *        is addressed to to this slave.
  *
  * @return 0 = address does not match, 1 = address match
  */
    uint8_t MODBUS_is_for_me(uint16_t my_node_addr){

        uint8_t adr_1 = (ASCII_ASCII_hex_2_bin(MODBUS_str[1]);
        uint8_t adr_0 = (ASCII_hex_2_bin(MODBUS_str[2]);

        uint8_t msg_addr = (adr_1 << 4) + adr_2;
        if (msg_addr == my_node_addr)
            return 0x01;
        else
            return 0x00;
    }




 /**
  * @brief A new MODBUS string is being evaluated.  Return the function code.
  * Defined codes include:
  *
  *      3 - read registers
  *     16 - write multiple registers
  *      6 - write single register
  *
  * @return the function code in the MODBUS message
  */
    uint_t MODBUS_get_function_code(void){

        uint8_t cmd_1 = (ASCII_hex_2_bin(MODBUS_str[3]);
        uint8_t cmd_0 = (ASCII_hex_2_bin(MODBUS_str[4]);

        uint8_t msg_addr = (cmd_1 << 4) + cmd_0;
    }




// Helper functions for "read registers mode"

    uint16_t MODBUS_get_addr(void);

        uint8_t addr_3 = (ASCII_hex_2_bin(MODBUS_str[5]);
        uint8_t addr_2 = (ASCII_hex_2_bin(MODBUS_str[6]);
        uint8_t addr_1 = (ASCII_hex_2_bin(MODBUS_str[7]);
        uint8_t addr_0 = (ASCII_hex_2_bin(MODBUS_str[8]);

        uint16_t starting_addr;

        starting_addr = (addr_3 << 4) + addr_2;
        starting_addr = starting_addr << 8;
        starting_addr += (addr_1 << 4) + addr_0;

        return starting_addr;
    }


    uint16_t MODBUS_number_of_reg_to_read(read);

        uint8_t num_3 = (ASCII_hex_2_bin(MODBUS_str[9]);
        uint8_t num_2 = (ASCII_hex_2_bin(MODBUS_str[10]);
        uint8_t num_1 = (ASCII_hex_2_bin(MODBUS_str[11]);
        uint8_t mum_0 = (ASCII_hex_2_bin(MODBUS_str[12]);

        uint16_t num;

        num = (num_3 << 4) + num_2;
        num = num << 8;
        num += (addr_1 << 4) + num_0;

        return num;
    }


// Helper functions for write multiple registers mode

    // FIXME develop these function when you have time






// Functions for "write single register" mode


    void MODBUS_echo(void){

        // FIXME add appropriate delays here
        USART_puts(MODBUS_str);

    }




    uint16_t MODBUS_get_reg_to_write(void){
         return MODBUS_get_addr();
    }



    uint16_t MOSBUS_get_data_to_write(void){

        uint8_t d_3 = (ASCII_hex_2_bin(MODBUS_str[9]);
        uint8_t d_2 = (ASCII_hex_2_bin(MODBUS_str[10]);
        uint8_t d_1 = (ASCII_hex_2_bin(MODBUS_str[11]);
        uint8_t d_0 = (ASCII_hex_2_bin(MODBUS_str[12]);

        uint16_t d;

        d = (d_3 << 4) + d_2;
        d = d << 8;
        d += (d_1 << 4) + d_0;

        return d;

    }
