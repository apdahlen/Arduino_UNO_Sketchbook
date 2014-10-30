#ifndef _ADC_H

    #define _ADC_H
    #include <stdint.h>
    #include <avr/io.h>

    void ADC_init(void);
    uint16_t read_ADC(uint8_t channel);
    void consecutive_ADC_read(uint16_t *P, uint8_t N);

#endif

