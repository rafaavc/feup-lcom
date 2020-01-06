#include <lcom/lcf.h>

/**
 * @brief Function to get time from the real time clock
 * 
 * Time comes in an array with 3 bytes in format [hours, minutes, seconds]
 */
void get_time_rtc();

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
