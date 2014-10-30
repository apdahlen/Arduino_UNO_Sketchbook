#include <Arduino.h>

// AVR Libc includes
    #include <avr/io.h>

    #include <stdint.h>

    #include "SPI_master.h"

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

/** SPI_master_init
 
 * @brief Initialize the AVR SPI peripheral as master.
 *
 * @param none
 *
 * @return none
 *
 * @note pin assignments.  On an Arduino UNO:
 *
 *      pin:    function:
 *
 *      10      CS_not          // In master mode this is not part of the SPI.  This particular
 *                              // pin was selected for its close proximity to the other SPI pins.
 *      11      MOSI
 *      12      MISO
 *      13      SCK
 */
void SPI_master_init(void){

    /* Set MOSI and SCK output, all others input */
        DDR_SPI |= (1 << DD_MOSI) | (1 << DD_SCK) | (1 << DD_CS_not);
        CS_idle;
    /* Enable SPI, Master, set clock rate fck/128 */
        SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
}



/** SPI__master_xfr
 *
 * @brief Transfer date to and from the SPI device
 *
 * @param N number of bytes to be transfered
 *
 * @param *SPI_tx_buf address of the buffer holding the data to be transmitted
 *
 * @param *SPI_rx_buf address of the buffer that will hold the incoming data
 *
 * @return none
 *
 */
void SPI_master_xfr(uint8_t N, uint8_t *SPI_tx_buf, uint8_t *SPI_rx_buf){

    int i;
    CS_assert;

    for (i = 0; i < N; i++){
        SPI_data_reg = *SPI_tx_buf;
        Wait_for_XMT;
        *SPI_rx_buf = SPI_data_reg;
        SPI_tx_buf ++;
        SPI_rx_buf ++;
    }
    CS_idle;

    for(i = 0; i < 255 ; i++); 
}

