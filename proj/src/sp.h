#include <lcom/lcf.h>

/** @defgroup SerialPort Serial Port
 * @{
 * Contains functions for serial port
 */

/**
 * @brief Subcribes interruptions for serial port (COM1)
 * 
 * @param bit_no Serial port hook id
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
 * @param bit_no Serial port hook id
 * @return 0 upon success and non-zero otherwise
 */
int (com2_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubcribes interruptions for serial port (COM2)
 * 
 * @return 0 upon success and non-zero otherwise
 */
int (com2_unsubscribe_int)();

/**
 * @brief Obtains the value of the divisor latch
 * 
 * @param base - the address of the port (COM1 or COM2)
 * @param dl - the address that will receive the value of the dl
 * 
 * @return 0 upon success and non-zero otherwise
 */
int sp_get_divisor_latch(unsigned base, unsigned *dl);

/**
 * @brief Writes to the divisor latch, setting the frame rate
 * @param base the address of the port (COM1 or COM2)
 * @param bitrate the bitrate to set
 * @return 0 upon success and non-zero otherwise
 */
int sp_set_bitrate(unsigned base, unsigned bitrate);

/**
 * @brief Configues the serial port
 * @param base the address of the port (COM1 or COM2)
 * @param word_length word size
 * @param stop_bits stop bit number
 * @param parity  parity bits
 * @param bitrate the bitrate to set
 * 
 * @return 0 upon success and non-zero otherwise
 */
int sp_set_conf(unsigned base, unsigned word_length, unsigned stop_bits, unsigned parity, unsigned bitrate);

/**
 * @brief Prints the configuration of the serial port
 * @param base the address of the port (COM1 or COM2)
 * @return 0 upon success and non-zero otherwise
 */
int sp_print_conf(unsigned base);

/**
 * @brief Writes to the interrupt enable register (IER), enabling the desired interrupts
 * @param base the address of the port (COM1 or COM2)
 * @param host whether this machine is the host or not
 * @return 0 upon success and non-zero otherwise
 */
int sp_enable_interrupts(unsigned base, bool host);

/**
 * @brief Writes to the interrupt enable register (IER), disabling the interrupts
 * @param base the address of the port (COM1 or COM2)
 * @param host whether this machine is the host or not
 * @return 0 upon success and non-zero otherwise
 */
int sp_disable_interrupts(unsigned base, bool host);

/**
 * @brief sets the fifo's up
 * @param base the address of the port (COM1 or COM2)
 * @return 0 upon success and non-zero otherwise
 */
int sp_setup_fifo(unsigned base);

/**
 * @brief prints the Line Status Register (LSR)
 * @param base the address of the port (COM1 or COM2)
 * @return 0 upon success and non-zero otherwise
 */
int sp_print_lsr(unsigned base);

/**
 * @brief GET method for the Line Status Register (LSR)
 * @param base the address of the port (COM1 or COM2)
 * @return the LSR
 */
uint8_t sp_get_lsr(unsigned base);

/**
 * @brief Initiates serial port
 */
void sp_init();

/**
 * @brief Terminates serial port
 */
void sp_terminate();

/**
 * @brief Sends a character
 * @param c the character to send
 */
void sp_send_character(uint8_t c);

/**
 * @brief Pushes 3 zero's to the transmission queue
 */
void charqueue_push_end_characters();

/**
 * @brief Transmits player name
 */
void transmit_player_data();

/**
 * @brief Transmits a string
 * @param str the string to transmit
 * @param str_len the number of characters in the string
 */
void transmit_string(char * str, uint8_t str_len);

/**
 * @brief Transmits mouse bytes
 * @param mouse_data pointer to the mouse packet that needs to be transmitted
 * @param mouse_yvariance mouse variance in y axis
 * @param mouse_xvariance mouse variance in x axis
 */
void transmit_mouse_bytes(struct packet * mouse_data, unsigned mouse_xvariance, unsigned mouse_yvariance);

/**
 * @brief Transmits a critical event
 * @param type the critical event to transmit
 */
void transmit_critical_event(char * type);

/**
 * @brief Transmits a kbd code (we only use kbd codes with one byte)
 * @param kbd_code the kbd code to transmit
 */
void transmit_kbd_code(uint8_t kbd_code);

/**
 * @brief goes to reception queue and gets information out
 * @return a number that has a meaning (the meaning is commented inside the function)
 */
int retrieve_info_from_queue();

/**
 * @brief Interrupt handler for serial port
 * @param com the address of the port (COM1 or COM2)
 */
int sp_ih(unsigned com);   // Returns a value based on whether it is needed to handle events

/** @}*/
