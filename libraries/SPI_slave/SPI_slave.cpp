#include <Arduino.h>

// AVR Libc includes
    #include <avr/io.h>

    #include <stdint.h>

    #include "SPI_slave.h"

    #define DDR_SPI   DDRB
    #define SPI_port PORTB

    #define DD_CS_not DDB2
    #define DD_SCK    DDB5
    #define DD_MOSI   DDB3
    #define DD_MISO   DDB4

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


uint8_t SPI_slave_get(void){

    /* Wait for reception complete */
    while(!(SPSR & (1 << SPIF)));
    /* Return Data Register */
    return SPDR;
}


void SPI_slave_put(uint8_t c){

    uint8_t dummy;

    SPDR = 0x10;//c;

    /* Wait for reception complete */
    while(!(SPSR & (1 << SPIF)));

    //SPSR = SPSR & !(1 << SPIF);   try to slear IF flag
    dummy = SPDR;


  //  while(!(SPSR & (1 << SPIF)));
}


uint8_t SPI_slave_xfr(uint8_t c){

    SPDR = c;
    /* Wait for reception complete */
    while(!(SPSR & (1 << SPIF)));
    /* Return Data Register */
    return SPDR;
}

/*
ISR (SPI_STC_vect)
{

  // add to buffer if room
  if (pos < sizeof buf)
    {
    buf [pos++] = c;

    // example: newline means time to process buffer
    if (c == '\n')
      process_it = true;

    }  // end of room available
}  // end of interrupt routine SPI_STC_vect

*/



/*
void SPI_slave_ISR_handle(void){

    char c = SPDR;  // grab byte from SPI Data Register

   }

*/
