#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>

/** @defgroup Keyboard Keyboard
 * @{
 * Contains functions for keyboard
 */

/**
 * @brief keyboard interrupts handler
 * 
 * Reads keyboard data and saves it to kbd_code
 */
void (kbc_ih)();


/**
 * @brief Subscribes and enables Keyboard interrupts
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbd_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes Keyboard interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbd_unsubscribe_int)();


/**
 * @brief Sends command byte to status reg
 * 
 * @param mask the mask has the bits that it wants to change to enabled with value = 1    IF mask = 0: Nothing changes
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int send_statusreg_commandbyte(uint8_t mask);

/**
 * @brief Enables keyboard interrupts
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int enable_interrupts();

/** @}*/

#endif /* __KEYBOARD_H */
