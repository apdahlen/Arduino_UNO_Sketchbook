
#ifndef _ASCII_MODBUS

    #define _ASCII_MODBUS

// Common

    void MODBUS_init(uint8_t dir_pin, uint16_t timeout);

    #define size_of_cmd_lines           40

// MASTER

<<<<<<< HEAD
    #define MODBUS_READ                 0x03
    #define MODBUS_WRITE                0x06
=======
    #define READ_HOLDING_REGISTERS      0x03
    #define PRESET_SINGLE_REGISTER      0x06
    #define PRESET_MULTIPLE_REGISTERS   0x10
>>>>>>> fa5b7b8d15806125014d565311a4da9f2030b36a

    #define BUS_WRITE                   0x01
    #define BUS_READ                    0x00

    #define USART_TIMEOUT_MILLISECONDS  1000

    extern char MODBUS_cmd_line[size_of_cmd_lines];
    extern char MODBUS_reply_line[size_of_cmd_lines];

    uint8_t MODBUS_put_word(uint8_t physical_addr, uint16_t mem_addr, uint16_t data);
    uint8_t MODBUS_read_registers(uint16_t *destination, uint8_t physical_addr, uint16_t starting_mem_addr, uint16_t get_n_words );

// SLAVE

<<<<<<< HEAD
    // FIXME is would also be nice to have a put multiple words
=======
    #define MODBUS_STR_LENGTH       100

    uint8_t MODBUS_slave_is_new_msg(void);

    void  MODBUS_slave_echo(void);

    uint16_t MODBUS_get_Nth_word(uint8_t N);
    uint8_t MODBUS_get_Nth_int(uint8_t N);

    void MODBUS_buffer_words(uint16_t index, uint16_t D);
    void MODBUS_put_N_words(uint8_t N, uint8_t slave_addr);
>>>>>>> fa5b7b8d15806125014d565311a4da9f2030b36a

#endif

