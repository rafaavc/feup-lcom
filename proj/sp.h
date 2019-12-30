#include <lcom/lcf.h>

/**
 * @brief Subcribes interruptions for serial port (COM1)
 * 
 * @param uint8_t* Serial port hook id
 * @return 0 upon success and non-zero otherwise
 */
int (com1_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubcribes interruptions for serial port (COM1)
 * 
 * @return 0 upon success and non-zero otherwise
 */
int (com1_unsubscribe_int)();

/**
 * @brief Subcribes interruptions for serial port (COM2)
 * 
 * @param uint8_t* Serial port hook id
 * @return 0 upon success and non-zero otherwise
 */
int (com2_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubcribes interruptions for serial port (COM2)
 * 
 * @return 0 upon success and non-zero otherwise
 */
int (com2_unsubscribe_int)();

int sp_get_divisor_latch(unsigned base, unsigned *dl);

int sp_set_bitrate(unsigned base, unsigned bitrate);

int sp_set_conf(unsigned base, unsigned word_length, unsigned stop_bits, unsigned parity, unsigned bitrate);

int sp_print_conf(unsigned base);

int sp_enable_interrupts(unsigned base);

int sp_setup_fifo(unsigned base);

int sp_print_lsr(unsigned base);

void sp_init();

void sp_terminate();

void sp_send_character(char c, bool reverse);

void sp_ih(unsigned mode, unsigned com_no);
