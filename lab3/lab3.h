#pragma once


#include <stdint.h>




/**
 * @brief Tests reading of scancodes via KBD interrupts
 *
 * Displays the scancodes received from the keyboard using interrupts.
 * Exits upon release of the ESC key
 * 
 * Must be defined using parenthesis around the function name:
 * 
 *   int (kbd_test_scan)() {
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbd_test_scan)();



/**
 * @brief Tests reading of scancodes via polling
 *
 * Displays the scancodes received from the keyboard using polling.
 * Exits upon release of the ESC key
 * 
 * Must be defined using parenthesis around the function name:
 * 
 *   int (kbd_test_poll)() {
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbd_test_poll)();



/**
 * @brief Tests handling of more than one interrupt
 *
 * Similar to kbd_test_scan() except that it should terminate also if no scancodes are received fo
 * n seconds
 * 
 * Must be defined using parenthesis around the function name:
 * 
 *  int (kbd_test_timed_scan)(uint8_t n) {
 * 
 * @parameters n Number of seconds without scancoodes before exiting
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbd_test_timed_scan)(uint8_t n);


/**
 * @brief Handles keyboard interrupts
 *
 * Reads the status register and the output buffer (OB).
 * If there was some error, the byte read from the OB should be discarded.
 * 
 * All communication with other code must be done via global variables, static if possible.
 * 
 * Must be defined using parenthesis around the function name:
 * 
 *     void (kbc_ih)(void) {
 * 
 * @parameters IH's take no arguments
 * @return IH's return no values
 */
void (kbc_ih)(void);


/**
 * @brief Prints the input scancode
 *
 * Provided via the LCF – no need to implement it
 * 
 * @parameters make Whether this i sa make or break code
 * @parameters size Size in bytes of the scancode
 * @parameters bytes Array with size elements, with the scancode bytes
 * @return 0 upon success and non-zero otherwise
 */
int (kbd_print_scancode)(bool make, uint8_t	size, uint8_t * bytes);



/**
 * @brief Prints the no. of sys_inb() calls.
 *
 * Provided via the LCF – no need to implement it
 * 
 * @parameters cnt Number of sys_inb() calls
 * @return 0 upon success and non-zero otherwise
 */
int (kbd_print_no_sysinb)(uint32_t cnt);

/**
 * @brief Handles keyboard polling
 * 
 * @return 0 upon success and non-zero otherwise
 */
int polling();

