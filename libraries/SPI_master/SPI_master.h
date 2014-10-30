#ifndef _SPI_master_H

    #define _SPI_master_H
    #include <stdint.h>



    // #define SPI_prefix 0xAA				// signal start of SPI command
    // #define SPI_XMT	0x02				// identify as broadcast from master
    // # define SPI_RCV 0x03				// identify as request from master

    void SPI_master_init(void);
    void SPI_master_xfr(uint8_t N, uint8_t *SPI_tx_buf, uint8_t *SPI_rx_buf);

#endif
