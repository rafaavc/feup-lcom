#include <lcom/lcf.h>

/** @defgroup RealTimeClock Real Time Clock
 * @{
 * Contains functions for RTC
 */

/**
 * @brief Function to get time from the real time clock
 * 
 * Time comes in an array with 3 bytes in format [hours, minutes, seconds]
 * @return true if it's not updating time and false otherwise
 */
bool get_time_rtc();

/**
 * @brief checks if date is on BCD
 * @return true if time is on BCD and false otherwise
 */
bool check_BCD();

/**
 * @brief changes number in BCD format to binary
 * @param number number to change
 */
void bcd_to_binary(uint8_t* number);

/**
 * @brief Subrscribes RTC interrupts for updates
 * @param bit_no bit number for policy
 * @return 0 upon success and 1 otherwise
 */
int rtc_subscribe_int(uint8_t* bit_no);

/**
 * @brief Unsubscribes RTC interrupts
 * @return 0 upon success and non-zero otherwise 
 */
int (rtc_unsubscribe_int)();

/**
 * @brief reads data from the RTC
 * @param addr address to read from
 * @param ret variable to store the info
 * @return 0 upon success and non-zero otherwise 
 */
int read_rtc(uint32_t addr, uint8_t *ret);

/**
 * @brief writes a command to the RTC
 * @param addr address to write to
 * @param value command to write
 * @return 0 upon success and non-zero otherwise 
 */
int write_rtc(uint32_t addr, uint8_t value);

/**
 * @brief writes the mode and the content that the user chose on settings menu
 * @param mode "A" if automatic (light mode during day and dark mode during night) or "C" if custom
 * @param content 0x0 if automatic mode and "T" for dark mode and "F" for light mode
 * @return 0 upon success and non-zero otherwise 
 */
int write_dark_mode(uint8_t mode, uint8_t content);

/**
 * @brief changes color mode automatically
 * @return 0 upon success and non-zero otherwise 
 */
int make_dm_auto();

/**
 * @brief changes color mode acordingly to player choice
 * @param dm true if changing to dark mode and false otherwise
 * @return 0 upon success and non-zero otherwise 
 */
int make_dm_custom(bool dm);

/** @}*/
