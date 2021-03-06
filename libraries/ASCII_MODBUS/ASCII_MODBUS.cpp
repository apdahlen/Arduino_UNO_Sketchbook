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

    #include "ASCII_MODBUS.h"
    #include "USART.h"
    #include "error.h"


// Public variables defined

    char MODBUS_cmd_line[size_of_cmd_lines];
    char MODBUS_reply_line[size_of_cmd_lines];


// Private functions

    void byte_array_2_str(char *line, uint8_t length, uint8_t *hex_array);
    uint16_t LRC_gen(uint8_t *data, uint8_t length);
    void pack_ASCII_str(char *line, uint8_t *c, uint8_t length);
    uint8_t ASCII_hex_2_bin(char c);


// Private alias


// Private variables

    char digit[ ] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    static uint8_t RS_485_dir_pin;
    static uint8_t USART_timeout_millieseconds;




/**
 * @brief Initialize the MODBUS - encapsulate the USART configuration.
 *
 * @param dir_pin declare the pin used to control the RS-485 transceiver
 *
 * @param timeout set the amount of time (in milliseconds) for the slave to respond.
 *
 * @Warning The USART_ISR must still be called from the main Arduino sketch.
 *
 *      ISR(USART_RX_vect){
 *          USART_handle_ISR();
 *      }
 *
 * @warning The ASCII LF is used as the terminator.  Don't forget the ASCII CR
 *          is part of MODBUS and will be held in the buffer.
 */
    void MODBUS_init(uint8_t dir_pin, uint16_t timeout){
        RS_485_dir_pin = dir_pin;
        digitalWrite(dir_pin, LOW);
        pinMode(dir_pin, OUTPUT);
        USART_timeout_millieseconds = timeout;
        USART_init_full(16000000ul, 19200l, 0x07, 'E');
        USART_set_terminator(0x0A);                                 // ASCII LF

    }




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
 * @param N_char number of characters in the input string.  This parameter is
 *        required as the input string may contain the NULL char.
 *
 * @note reference the GS1 documentation for more information.
 */

    void pack_ASCII_str(char *line, uint8_t *c, uint8_t N_char){

        uint8_t LRC = LRC_gen(c, N_char);

        *line++ = ':';

        byte_array_2_str(line, N_char, c);
        line+= N_char << 1;
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
 *         : 01 06 0100 1770 71 CR LF   (spaces included to separate the fields)
 *
 * @param slave_addr a byte identifying a particular MODBUS device.  Note
 *        this must be manually programmed into a device such as the GS1.
 *
 * @param mem_addr a 16-bit value identifying the particular MODBUS register to
 *        be written
 *
 * @param data a 16-bit value containing the data to be written to mem_addr
 *
 * @return result of operation, 1 = success, 0 = failure
 *
 * @note  There is a glitch as the RS-485 transceiver transitions from XMT to
 *        RCV.  This delay moves the transition outside of the GS1's observation
 *        window.  Note that the GS1 takes approximately 2.5 mS to start reply.
 */

    uint8_t MODBUS_put_word(uint8_t slave_addr, uint16_t mem_addr, uint16_t data){

        #define match 0x00

        uint8_t mem_addr_h = mem_addr >> 8;
        uint8_t mem_addr_l = mem_addr & 0x00FF;

        uint8_t data_h = data >> 8;
        uint8_t data_l = data & 0x00FF;

        uint8_t cmd_str_hex[] = { slave_addr, PRESET_SINGLE_REGISTER, mem_addr_h, mem_addr_l, data_h, data_l } ;

        uint16_t milisecond_cnt;

        pack_ASCII_str(MODBUS_cmd_line, cmd_str_hex, 6);

    // Write the word

        digitalWrite(RS_485_dir_pin, BUS_WRITE);
        delayMicroseconds(1000);
        USART_puts(MODBUS_cmd_line);
        delayMicroseconds(1500);
        digitalWrite(RS_485_dir_pin, BUS_READ);

    // Verify the word was written by analyzing the echo

        milisecond_cnt = 0;
        while(!USART_is_string( )){
            delay(1);
            if (++milisecond_cnt > USART_timeout_millieseconds){                                // prevent lockup if device is not connected
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
 * @param slave_addr a byte identifying a particular MODBUS device.  Note this
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

   uint8_t MODBUS_read_registers(uint16_t *destination, uint8_t slave_addr, uint16_t starting_mem_addr, uint16_t get_n_words ) {

        uint8_t starting_mem_addr_h = starting_mem_addr >> 8;
        uint8_t starting_mem_addr_l = starting_mem_addr & 0x00FF;

        uint8_t get_n_words_h = get_n_words >> 8;
        uint8_t get_n_words_l = get_n_words & 0x00FF;

        uint8_t cmd_str_hex[] = { slave_addr, READ_HOLDING_REGISTERS, starting_mem_addr_h, starting_mem_addr_l, get_n_words_h, get_n_words_l } ;

        uint16_t milisecond_cnt;

        pack_ASCII_str(MODBUS_cmd_line, cmd_str_hex, 6);

    // Send the request for N words of data

        digitalWrite(RS_485_dir_pin, BUS_WRITE);
        delayMicroseconds(1000);
        USART_puts(MODBUS_cmd_line);
        delayMicroseconds(1500);
        digitalWrite(RS_485_dir_pin, BUS_READ);

    // Verify the word was written by analyzing the echo

        milisecond_cnt = 0;
        while(!USART_is_string( )){
            delay(1);
            if (++milisecond_cnt == USART_TIMEOUT_MILLISECONDS){         // prevent lockup if device is not connected
                strncpy(ERROR_MSG, "MODBUS_read_reg: USART timeout", SIZE_ERROR_MSG);
                return 0x00;
            }
        }

        USART_gets(MODBUS_reply_line);

    // Verify first 5 ASCII characters of response are correct

        if(strncmp(MODBUS_cmd_line, MODBUS_reply_line, 5) != match){   // limit to the first 15 characters (no need to test the line terminators)
            strncpy(ERROR_MSG, "MODBUS_read_reg: first 5 characters don't match", SIZE_ERROR_MSG);
            return 0x00;
        }

    // Verify the number bytes received equals number of bytes requested

        uint16_t num_bytes_received = (ASCII_hex_2_bin(MODBUS_reply_line[5]) << 8) + ASCII_hex_2_bin(MODBUS_reply_line[6]);

        if (num_bytes_received !=  (get_n_words << 1)){
            strncpy(ERROR_MSG, "MODBUS_read_reg: improper number words returned", SIZE_ERROR_MSG);
            return 0x00;
        }

    // Put the received words into the destination buffer

        uint16_t temp;

        for (uint16_t i = 0; i < get_n_words * 4; i = i + 4){

            temp =  ASCII_hex_2_bin(MODBUS_reply_line[i + 7]) << 12;
            temp += ASCII_hex_2_bin(MODBUS_reply_line[i + 8]) << 8;
            temp += ASCII_hex_2_bin(MODBUS_reply_line[i + 9]) << 4;
            temp += ASCII_hex_2_bin(MODBUS_reply_line[i + 10]);

            *destination = temp;
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


    static char MODBUS_str[size_of_cmd_lines];
    static uint8_t num_char;


/**
* @brief Convert an ASCII codex hex character into its integer equivalent
*
* @param a hex character in ASCII form
*
* @return an unsigned integer
*/
    uint8_t ASCII_hex_2_bin(char c) {

        if (c <= '9'){
            return c - '0';
        }
        return (c - 'A') + 10;

    }









 /**
  * @brief Determine if a valid MODBUS message has been received.  On successful retrieval the 
  * message is held in a local MODBUS_str[] buffer.
  *
  * @return 0 = no new message, 1 = a valid message had been retrieved
  */
    uint8_t MODBUS_slave_is_new_msg(void){

        if (!USART_is_string())
            return 0x00;

        num_char =  USART_gets(MODBUS_str);

        // TODO add code to verify the LRC

        return 0x01;

    }






/**
 * @brief MODBUS mode 6 is used to preset a single register.  The slave replies with an echo of the
 * original message.
 *
 * @note Note the CR character is already contained in the received string since the LF was used as
 * the terminating character.
 *
 * @note For debug purposes the complete MODBUS_cmd_line is available.  This function could be 
 * improved by removing this feature.
 */

    void MODBUS_slave_echo(void){

        char term_str[] = {0x0A, 0x00};                         // end with CR (already on string) LF, NULL

        strncpy(MODBUS_cmd_line, MODBUS_str, size_of_cmd_lines);    
        strncat (MODBUS_cmd_line, term_str, size_of_cmd_lines);

        digitalWrite(RS_485_dir_pin, BUS_WRITE);
        delayMicroseconds(1000);
        USART_puts(MODBUS_cmd_line);
        delayMicroseconds(1500);
        digitalWrite(RS_485_dir_pin, BUS_READ);

    }





/**
 * @brief The incoming MODBUS string is held in a line buffer called MODBUS_str.  This function is
 * used to pull a single word (16-bit value) from the buffer.  Recall that the incoming string
 * consists of hexadecimal encoded ASCII characters.  This function combines 4 such characters.
 *
 * @param N is a pointer to the starting position of the desired word.
 *
 * @return the 16-bit value.  For example the specified data address and register data are 
 * encoded as 16-bit values.
 */
    uint16_t MODBUS_get_Nth_word(uint8_t N){

        uint8_t d_3 = (ASCII_hex_2_bin(MODBUS_str[N    ]));
        uint8_t d_2 = (ASCII_hex_2_bin(MODBUS_str[N + 1]));
        uint8_t d_1 = (ASCII_hex_2_bin(MODBUS_str[N + 2]));
        uint8_t d_0 = (ASCII_hex_2_bin(MODBUS_str[N + 3]));

        uint16_t d;

        d = (d_3 << 4) + d_2;
        d = d << 8;
        d += (d_1 << 4) + d_0;

        return d;

    }


/**
 * @brief The incoming MODBUS string is held in a line buffer called MODBUS_str.  This function is
 * used to pull a single integer (8-bit value) from the buffer.  Recall that the incoming string
 * consists of hexadecimal encoded ASCII characters.  This function combines 2 such characters.
 *
 * @param N is a pointer to the starting position of the desired integer.
 *
 * @return the 8-bit value.  For example, the second field in a MODBUS string is a 8-bit 
 * slave address.
 */
    uint8_t MODBUS_get_Nth_int(uint8_t N){

        uint8_t d_1 = (ASCII_hex_2_bin(MODBUS_str[N    ]));
        uint8_t d_0 = (ASCII_hex_2_bin(MODBUS_str[N + 1]));

        return (d_1 << 4) + d_0;

    }



#define N_REGS 50

uint16_t regs[N_REGS];


/**
 * @brief This function is involved is assembling an outgoing MODBUS frame.  When this function is
 * called the values to be sent have already been collected into the buffer regs.  This function
 * prepends the slave address, MODBUS function code, and number of bytes to be sent.  It them calls
 * the pack_ASCII_str function which completes the frame assembly by prepending the ':' symbol
 * and appending the LRC and CR/LF pair.
 *
 * @param N number of words (16-bit) to be included in the frame.  
 *
 * @param slave_addr The address of the sending slave
 *
 * @Caution In the response message number of data is specified in bytes.
 */

    void MODBUS_put_N_words(uint8_t N, uint8_t slave_addr){

        uint8_t cmd_str_hex[40] = { slave_addr, READ_HOLDING_REGISTERS, N * 2 } ;            //FIXME Test tomorrow - this may have been the error

        uint8_t i;

        for(i = 0; i < N; i++){                             // take 16-bit words stored in regs and split into 8-bit

            cmd_str_hex[(i * 2) + 3] = regs[i] >> 8;
            cmd_str_hex[(i * 2) + 4] = regs[i] & 0x00FF;

        }

        pack_ASCII_str(MODBUS_cmd_line, cmd_str_hex, 3 + (N * 2));

    // Write the word

        digitalWrite(RS_485_dir_pin, BUS_WRITE);
        delayMicroseconds(1700);
        USART_puts(MODBUS_cmd_line);
        delayMicroseconds(1500);
        digitalWrite(RS_485_dir_pin, BUS_READ);
    }


/**
 * @brief An outgoing MODBUS frame is built in a temporary buffer called regs.  This setter function
 * is used to place a new 16-bit value in the desired position.
 *
 * @param index specified where the data is to be placed
 *
 * @param D contains the data
 *
 * @note This function is designed to be called from within a loop for example:
 *
 * @code
 *        case 0x0001:
 *
 *           for(i = 0; i < n; i++){
 *               MODBUS_buffer_words(i, i + 1);
 *           }
 *           MODBUS_put_N_words(n, MY_ADDR);
 *
 *           break;
 *
 *  @endcode
 */

    void MODBUS_buffer_words(uint16_t index, uint16_t D){

        regs[index] = D;
    }
