#include <lcom/lcf.h>

/**
 * @brief Subcribes interruptions for serial port
 * 
 * @param uint8_t* Serial port hook id
 * @return 0 upon success and non-zero otherwise
 */
int (com1_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unubcribes interruptions for serial port
 * 
 * @return 0 upon success and non-zero otherwise
 */
int (com1_unsubscribe_int)();

int sp_get_divisor_latch(unsigned base, unsigned *dl);

int sp_set_bitrate(unsigned base, unsigned bitrate);

int sp_set_conf(unsigned base, unsigned word_length, unsigned stop_bits, unsigned parity, unsigned bitrate);

int sp_print_conf(unsigned base);

int sp_enable_interrupts(unsigned base);

int sp_setup_fifo(unsigned base);

int sp_print_lsr(unsigned base);

void com1_ih();
