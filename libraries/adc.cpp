
    #include "adc.h"
    #include <stdint.h>
    #include <avr/io.h>

/**
 * @brief Initialize the ADC. For an Arduino UNO with a 16 MHZ clock:
 *
 *  An ADC conversion takes 13 ADC clock cycles.
 *
 *      Prescaler | ADC clock | Max sample rate | resolution
 *      ----------|-----------|-----------------|-----------
 *         128    |  125 kHz  |  9.6 kHz        |   10 bits
 *          64    |  250 kHz  | 15.6 kHz        |   < 10
 *          32    |  500 kHz  | 38.5 kHz        |   < 10
 *
 *
 *                         V_in x 1024
 *           ADC_result = -------------
 *                            V_ref
 *
 * @ref
 *      http://www.atmel.com/images/atmel-8271-8-bit-avr-microcontroller-atmega48a-48pa-88a-88pa-168a-168pa-328-328p_datasheet.pdf
 *      http://www.openmusiclabs.com/learning/digital/atmega-adc/
 *      http://arduino.cc/en/Reference/analogRead
 *      http://www.atmel.com/Images/Atmel-8456-8-and-32-bit-AVR-Microcontrollers-AVR127-Understanding-ADC-Parameters_Application-Note.pdf
 *      http://web.engr.oregonstate.edu/~traylor/ece473/pdfs/freaks_DN_021.pdf
 *      http://www.atmel.com/images/doc8003.pdf
 */
void ADC_init(){

  //ADMUX |= (1 << REFS0);                              // VCC as voltage reference
    ADMUX |= (1 << REFS1) | (1 << REFS0);               // Internal 1.1 V as voltage reference

    ADCSRA &= 0xF8;                                     // FIXME what about this line make the prescaler work?

  //ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);   // Prescaler = 128
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0);   // Prescaler = 64
  //ADCSRA |= (1 << ADPS2) | (0 << ADPS1) | (1 << ADPS0);   // Prescaler = 32

    ADCSRA |= (1 << ADEN);    // Enable the ADC

}



/**
 * @brief Perform a single read of the desired ADC channel.
 *
 * @param channel contains the desired ADC channel
 *
 * @return the result of the ADC conversion
 */

uint16_t read_ADC(uint8_t channel){

    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);          // select ADC channel with safety mask
    ADCSRA |= (1 << ADSC);                              // single conversion mode
    while( ADCSRA & (1 << ADSC) );                      // wait until ADC conversion is complete
    return ADC;
}




/**
 * @brief Perform sequential reads on 4 ADC channels.
 *
 * @param pointer to array to a 4 element uint16_t array to hold ADC results
 *
 * @param N number of channels to read where 1 < N < 8 (8 reads the temperature)
 *
 * @note For a system with 4 consecutive reads this function takes approximately 200 uS when:
 *
 *          * Arduino UNO with 16 MHz clock
 *          * prescaler = 64
 *          * ADC clocks per sample = 13
 *
 * @warning The full 10-bits resolution cannot be obtained with these settings.
 *
 */
void consecutive_ADC_read(uint16_t *P, uint8_t N){

//  #define cap_charge_delay 10
    uint8_t i, j;

    if (N > 8)                                          // allow ADC channels 0 through 7 plus the temperature sensor
        N = 8;

    for (j = 0; j < N; j++){

        ADMUX = (ADMUX & 0xF0) | (j & 0x0F);            // select channel
       // for(i = 0; i < cap_charge_delay ; i++);       // FIXME should we allow extra time for capacitor to charge
        ADCSRA |= (1 << ADSC);                          // start conversion
        while( ADCSRA & (1 << ADSC) );                  // wait until ADC conversion is complete
        *P = ADC;
        P++;
    }
}
