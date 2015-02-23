
#ifndef _ASCII_MODBUS

    #define _ASCII_MODBUS

// Common

    void MODBUS_init(uint8_t dir_pin, uint8_t timeout);

// MASTER

    #define MODBUS_READ             0x03
    #define MODBUS_WRITE            0x06

    #define BUS_WRITE               0x01
    #define BUS_READ                0x00

    #define USART_TIMEOUT_MILLISECONDS  100

    #define size_of_cmd_lines           40

    extern char MODBUS_cmd_line[size_of_cmd_lines];
    extern char MODBUS_reply_line[size_of_cmd_lines];

    uint8_t MODBUS_put_word(uint8_t physical_addr, uint16_t mem_addr, uint16_t data);
    uint8_t MODBUS_read_registers(uint16_t *destination, uint8_t physical_addr, uint16_t starting_mem_addr, uint16_t get_n_words );

// SLAVE

    #define MODBUS_STR_LENGTH       100



    // FIXME is would also be nice to have a put multiple words

#endif

