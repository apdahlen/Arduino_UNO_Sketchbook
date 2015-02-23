
#ifndef _ASCII_MODBUS

    #define _ASCII_MODBUS

// Common

    void MODBUS_init(uint8_t dir_pin, uint8_t timeout);

// MASTER

    #define READ_HOLDING_REGISTERS      0x03
    #define PRESET_SINGLE_REGISTER      0x06
    #define PRESET_MULTIPLE_REGISTERS   0x10

    #define BUS_WRITE               0x01
    #define BUS_READ                0x00

    #define USART_TIMEOUT_MILLISECONDS  1000

    #define size_of_cmd_lines           40

    extern char MODBUS_cmd_line[size_of_cmd_lines];
    extern char MODBUS_reply_line[size_of_cmd_lines];

    uint8_t MODBUS_put_word(uint8_t physical_addr, uint16_t mem_addr, uint16_t data);
    uint8_t MODBUS_read_registers(uint16_t *destination, uint8_t physical_addr, uint16_t starting_mem_addr, uint16_t get_n_words );

// SLAVE

    #define MODBUS_STR_LENGTH       100

    uint8_t MODBUS_slave_is_new_msg(void);
    uint8_t MODBUS_is_for_me(uint16_t my_node_addr);
    uint8_t MODBUS_slave_get_function_code(void);
    uint16_t MODBUS_slave_get_addr(void);
    uint16_t MODBUS_slave_number_of_reg_to_read(void);
    uint16_t MODBUS_slave_get_single_reg(void);
    void  MODBUS_slave_echo(void);

#endif
