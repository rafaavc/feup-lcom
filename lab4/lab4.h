#pragma once


#include <stdint.h>


enum state{
  mSTART,
  mLB_PRESSED,
  mL_LINE_DRAWN,
  mLB_RELEASED,
  mRB_PRESSED,
  mR_LINE_DRAWN,
  mRB_RELEASED
};

enum event{
  mBEGIN,
  mLB_PRESS,
  mDRAW_L_LINE,
  mLB_RELEASE,
  mRB_PRESS,
  mDRAW_R_LINE,
  mRB_RELEASE,
  mOTHER = 99
};

enum state jump_state(enum state s, enum event e);

/**
 * @brief To test packet reception via interrupts.
 * Enables data reporting in streaming mode by the mouse
 * Reads, parses and displays the packets received from the mouse
 * Returns after receiving the number of packets specified in its argument
 * Disables data reporting before returning
 * Must be defined using parenthesis around the function name.
 * 
 * @param cnt Number of packets to receive and display before exiting
 * 
 * @return 0 upon succes and non-zero otherwise
 */
int (mouse_test_packet)	(uint32_t 	cnt);



/**
 * @brief To test PS/2 remote mode operation.
 * Configures the PS/2 to operate in remote mode
 * Periodically requests a packet from the mouse,
 * and parses and displays the packets received from the mouse
 * Returns after receiving the number of packets specified in argument.
 * Before returning, sets streaming mode and disables data reporting and 
 * resets the KBC command byte to Minix's default value
 * Must be defined using parenthesis around the function name.
 * 
 * @param period Period in milliseconds
 * @param cnt Number of packets to receive and display before exiting
 * 
 * @return 0 upon success and non-zero otherwise
 */
int (mouse_test_remote)(uint16_t period, uint8_t cnt);


/*
To test handling of more than one interrupt.

Similar to test_packet(), except that it 
should return if no packets are received for idle_time seconds
rather than after the reception of a given number of packets

Must be defined using parenthesis around the function name.
*/
/**
 * @brief To test handling of more than one interrupt.
 * Similar to test_packet(), except that it 
 * should return if no packets are received for idle_time seconds
 * rather than after the reception of a given number of packets
 * Must be defined using parenthesis around the function name.
 * 
 * @param idle_time Number of seconds without packets before exiting
 * 
 * @return 0 upon success and non-zero otherwise 
 */
int (mouse_test_async)(uint8_t idle_time);


/*
To test state machine implementation.

Similar to mouse_test_packet() except that it 
should terminate if user executes a 
gesture to be defined with the mouse.

Disables data reporting before returning.

Must be defined using parenthesis around the function name.
*/
/**
 * @brief To test state machine implementation.
 * Similar to mouse_test_packet() except that it 
 * should terminate if user executes a 
 * gesture to be defined with the mouse.
 * Disables data reporting before returning.
 * Must be defined using parenthesis around the function name.
 * 
 * @param ... to be defined
 * 
 * @return 0 upon success and non-zero otherwise
 */
int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance);
