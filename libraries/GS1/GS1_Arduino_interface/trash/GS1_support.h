#ifndef GS1_support

    #define GS1_support

    #include <stdint.h>
    #include <string.h>



    #define GS1_READ            0x03                    // FIXME are these required
    #define GS1_WRITE           0x06
    #define GS1_write_multiple  0x10

    #define GS1_dev_addr        0x01

//Motor Parameter addresses

    #define mtr_name_volts      0x0000
    #define mtr_name_amps       0x0001
    #define mtr_base_freq       0X0002
    #define mtr_base_rpm        0x0003
    #define mtr_max_rpm         0x0004

// Ramp Parameter addresses

    #define stop_method         0x0100
    #define acceleration_time_1 0x0101
    #define deceleration_time_1 0x0102


    //FIXME add the remainder of GS1 addresses


    void GS1_set_speed(uint8_t GS1_physical_addr, uint16_t deci_freq);
 
    void GS1_turn_on(uint8_t GS1_physical_addr);

    void GS1_turn_off(uint8_t GS1_physical_addr);

#endif

