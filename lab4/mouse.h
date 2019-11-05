#include <lcom/lcf.h>


/*struct packet
{
  uint8_t bytes[3];
  bool rb, lb, mb, x_ov, y_ov;
  int16_t delta_x, delta_y;
};*/  


int (mouse_subscribe_int)(uint8_t *bit_no);


int (mouse_unsubscribe_int)();

int send_statusreg_commandbyte(uint8_t mask_enable, uint8_t mask_disable);

/**
 * @brief Sends command byte to status reg
 * 
 * @param mask_enable the mask has the bits that it wants to change to enabled with value = 1    IF mask_enable = 0: Nothing changes
 *        mask_disable the mask has the bits that it wants to change to disabled with value = 0    IF mask_disable = 0: Nothing changes
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int send_statusreg_command(uint8_t cmd, uint8_t args);

/*
Enables stream mode data reporting.

Enables stream mode data reporting, by sending the respective command to the mouse.

This function is provided by the LCF.

In order to score points for your Lab 4 grade, you should implement this functionality yourself. This can be done by defining your own function with the same functionality, or just by adding this functionality somewhere in your code
*/
int mouse_enable_data_reporting_original();	

int mouse_disable_data_reporting();


unsigned int check_data();


/*
Handles mouse interrutps.

Reads the status register and the output buffer (OB).

If there was some error, the byte read from the OB should be discarded.

Must be defined using parenthesis around the function name.
*/
void (mouse_ih)();

/**
 * @brief Does polling for mouse
 * 
 * @param period Time between each search in miliseconds
 * 
 * @return 0 upon success and non-zero otherwise
 */
int mouse_polling();
