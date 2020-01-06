#include <lcom/lcf.h>

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

int (rtc_unsubscribe_int)();

int read_rtc(uint32_t addr, uint8_t *ret);

int write_rtc(uint32_t addr, uint8_t value);
