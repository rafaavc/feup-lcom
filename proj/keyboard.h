#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>


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
 * @param mask_enable the mask has the bits that it wants to change to enabled with value = 1    IF mask_enable = 0: Nothing changes
 *        mask_disable the mask has the bits that it wants to change to disabled with value = 0    IF mask_disable = 0: Nothing changes
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

#endif /* __KEYBOARD_H */
