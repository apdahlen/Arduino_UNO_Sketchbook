#include <Arduino.h>

// AVR Libc includes
    #include <avr/io.h>

    #include <stdint.h>

    #include "SPI_slave.h"

    #define DDR_SPI   DDRB
    #define SPI_port PORTB

    #define DD_CS_not DDB2      // Arduino UNO pin 10
    #define DD_SCK    DDB5      //  ''     ''  pin 13
    #define DD_MOSI   DDB3      //  ''     ''  pin 11
    #define DD_MISO   DDB4      //  ''     ''  pin 12

    #define  READ_CMD   0x03
    #define  WRITE_CMD  0x02

    #define SPI_data_reg    SPDR
    #define Wait_for_XMT    while(!(SPSR & (1<<SPIF)))
    #define CS_assert       SPI_port &= ~(1 << DD_CS_not)
    #define CS_idle         SPI_port |= (1 << DD_CS_not)

/** SPI_slave_init
 *
 * @brief Initialize the AVR SPI peripheral as slave.
 *
 * @param none
 *
 * @return none
 *
 * @note pin assignments.  On an Arduino UNO:
 *
 *      pin:    function:
 *
 *      10      CS_not          //  Selected for its close proximity to the other SPI pins.
 *      12      MOSI
 *      11      MISO
 *      13      SCK
 */
void SPI_slave_init(void){
    /* Set MISO output, all others input */
        DDR_SPI = (1 << DD_MISO);
    /* Enable SPI */
        SPCR = (1<<SPE);
}


uint8_t SPI_slave_get_char(void){

    /* Wait for reception complete */
    while(!(SPSR & (1 << SPIF)));
    /* Return Data Register */
    return SPDR;

}


void SPI_slave_put_8(uint8_t c){

    uint8_t dummy;
    SPDR = c;
    while(!(SPSR & (1 << SPIF)));
    dummy = SPDR;                           // clear the SPI IF

}



void SPI_slave_put_16(uint16_t c){

    uint8_t dummy;
    SPDR = c >> 8;                          // upper byte sent first
    while(!(SPSR & (1 << SPIF)));
    dummy = SPDR;                           // clear the SPI IF
    SPDR = c;                               // lower byte sent
    while(!(SPSR & (1 << SPIF)));
    dummy = SPDR;                           // clear the SPI IF

}

