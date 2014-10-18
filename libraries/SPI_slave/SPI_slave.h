#ifndef _SPI_SLAVE_H

    #define _SPI_SLAVE_H

    #include <stdint.h>

   void SPI_slave_init(void);
char SPI_slave_get_char(void);
   // void SPI_close(void);

 //   void SPI_slave_transfer(uint8_t N, uint8_t *SPI_tx_buf, uint8_t *SPI_rx_buf);
   void SPI_slave_ISR(void);

#endif
