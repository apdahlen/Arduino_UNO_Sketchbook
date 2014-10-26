#ifndef _SPI_SLAVE_H

    #define _SPI_SLAVE_H

    #include <stdint.h>

    void SPI_slave_init(void);
    uint8_t SPI_slave_get(void);
    void SPI_slave_put_8(uint8_t);
    void SPI_slave_put_16(uint16_t);

#endif
