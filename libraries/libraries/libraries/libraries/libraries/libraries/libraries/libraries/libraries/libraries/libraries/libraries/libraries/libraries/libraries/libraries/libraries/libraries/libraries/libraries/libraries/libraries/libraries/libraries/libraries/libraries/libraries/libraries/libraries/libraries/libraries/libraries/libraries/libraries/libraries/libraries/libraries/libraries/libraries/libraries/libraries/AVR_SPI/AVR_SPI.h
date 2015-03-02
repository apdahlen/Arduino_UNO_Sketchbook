#ifndef _AVR_SPI

    #define _AVR_SPI
    #include <stdint.h>

    void AVR_SPI_master_init(void);
    void AVR_SPI_master_xfr(uint8_t N, uint8_t *SPI_tx_buf, uint8_t *SPI_rx_buf);

    void AVR_SPI_slave_init(void);
    uint8_t AVR_SPI_slave_get_char(void);
    void AVR_SPI_slave_put_8(uint8_t);
    void AVR_SPI_slave_put_16(uint16_t);

#endif

