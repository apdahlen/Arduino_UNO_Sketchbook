#ifndef _SPI_SLAVE_H

    #define _SPI_SLAVE_H

    #include <stdint.h>

    void SPI_slave_init(void);
    uint8_t SPI_slave_get(void);
    void SPI_slave_put(uint8_t);

#endif
