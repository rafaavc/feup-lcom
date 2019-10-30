#pragma once

/** @defgroup utils utils
 * @{
 *
 * LCOM's utility functions
 */

#include <stdint.h>

#ifndef BIT
#  define BIT(n) (1 << (n))
#endif

/**
 * @brief Returns the LSB of a 2 byte integer
 *
 * @param val input 2 byte integer
 * @param lsb address of memory location to be updated with val's LSB
 * @return Return 0 upon success and non-zero otherwise
 */
int (util_get_LSB)(uint16_t val, uint8_t *lsb);

/**
 * @brief Returns the MSB of a 2 byte integer
 *
 * @param val input 2 byte integer
 * @param msb address of memory location to be updated with val's LSB
 * @return Return 0 upon success and non-zero otherwise
 */
int (util_get_MSB)(uint16_t val, uint8_t *msb);

/**
 * @brief Invokes sys_inb() system call but reads the value into a uint8_t variable.
 *
 * @param port the input port that is to be read
 * @param value address of 8-bit variable to be update with the value read
 * @return Return 0 upon success and non-zero otherwise
 */
int (util_sys_inb)(int port, uint8_t *value);


/**
 * @brief Return the MSbit of a byte intenger
 * 
 * @param val input 1 byte integer
 * @return Returns the MSbit
 */
uint8_t (util_get_MSbit)(uint8_t val);

/**
 * @brief Calculates Delta from MSB and corresponding byte
 * 
 * @param MSB Most significant bit
 * @param Byte Byte with first 8 bits
 * 
 * @return Delta
 */
int16_t (join_bytes)(uint8_t MSB, uint8_t byte);

