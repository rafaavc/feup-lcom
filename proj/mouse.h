#include <lcom/lcf.h>
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

/**
 * @brief Breaks down the 3 bytes received to struct packet variable
 * 
 * @param struct packet* mouse data with array of bytes received already 
 */
void parse_packet(struct packet *mouse_data);

/**
 * @brief Handles mouse interrutps.
 * 
 * Reads the status register and the output buffer (OB).
 * 
 * If there was some error, the byte read from the OB should be discarded.
 * 
 * Must be defined using parenthesis around the function name.
*/
void (mouse_ih)();

/**
 * @brief Handles timer interrutps.
 * 
 * Adds 1 to the global variable counter
*/
void (timer_int_handler)();

/**
 * @brief Does polling for mouse
 * 
 * @return 0 upon success and non-zero otherwise
 */
int mouse_polling();
