#include <lcom/lcf.h>
#include "lab4.h"

int (mouse_subscribe_int)(uint8_t *bit_no);
int (mouse_unsubscribe_int)();


int write_command_byte(uint8_t cmd);
int send_command_to_mouse(uint8_t);


int send_statusreg_command(uint8_t cmd, uint8_t args);

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
