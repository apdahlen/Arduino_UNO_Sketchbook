#ifndef _GS1_SUPPORT

    #define _GS1_SUPPORT

    #include <stdint.h>

// Motor Parameter addresses

    #define mtr_name_volts                  0x0000
    #define mtr_name_amps                   0x0001
    #define mtr_base_freq                   0X0002
    #define mtr_base_rpm                    0x0003
    #define mtr_max_rpm                     0x0004

// Ramp Parameter addresses

    #define stop_method                     0x0100
    #define acceleration_time_1             0x0101
    #define deceleration_time_1             0x0102

// TODO add the remainder of GS1 addresses or maybe gust git rid of all of the above...

//Communication Parameters - ref GS1 page 5-7

    #define Serial_Comm_Speed_Reference     0x091A
    #define Serial_Comm_RUN_Command         0x091B

    uint8_t GS1_init(uint8_t slave_address, uint8_t dir_pin, uint8_t timeout);

    uint8_t GS1_set_speed(uint8_t physical_addr, uint16_t deci_freq);

    uint8_t GS1_turn_on(uint8_t physical_addr);

    uint8_t GS1_turn_off(uint8_t physical_addr);

#endif
