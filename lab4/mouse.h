#include <lcom/lcf.h>
#include "lab4.h"

/**
 * @brief Subcribes interruptions for mouse
 * 
 * @param uint8_t* Mouse_hook_id
 * @return 0 upon success and non-zero otherwise
 */
int (mouse_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unubcribes interruptions for mouse
 * 
 * @return 0 upon success and non-zero otherwise
 */
int (mouse_unsubscribe_int)();

/**
 * @brief Writes a command through Output buffer
 * 
 * @param uint8_t command to write
 * @return 0 upon success and non-zero otherwise
 */
int write_command_byte(uint8_t cmd);

/**
 * @brief Sends a command through mouse Output buffer and verifies response
 * 
 * @param uint8_t command to write
 * @return 0 upon success and non-zero otherwise
 */
int send_command_to_mouse(uint8_t cmd);


void parse_packet(struct packet *mouse_data);

/*
Handles mouse interrutps.

Reads the status register and the output buffer (OB).

If there was some error, the byte read from the OB should be discarded.

Must be defined using parenthesis around the function name.
*/
void (mouse_ih)();

void (timer_int_handler)();

/**
 * @brief Does polling for mouse
 * 
 * @param period Time between each search in miliseconds
 * 
 * @return 0 upon success and non-zero otherwise
 */
int mouse_polling();
