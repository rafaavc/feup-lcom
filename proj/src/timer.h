/** @defgroup Timer Timer
 * @{
 * Contains functions for timer
 */

/**
 * @brief handles timer interrupts
 * 
 * increments both timer_counter and timer_counter_play
 */
void (timer_int_handler)();

/**
 * @brief Subscribes and enables Timer 0 interrupts
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int (timer_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(timer_unsubscribe_int)();

/** @}*/
