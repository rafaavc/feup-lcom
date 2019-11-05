#ifndef _LCOM_I8254_H_
#define _LCOM_I8254_H_

#include <lcom/lcf.h>

/** @defgroup i8254 i8254
 * @{
 *
 * Constants for programming the i8254 Timer. Needs to be completed.
 */

#define TIMER_FREQ 1193182 /**< @brief clock frequency for timer in PC and AT */
#define TIMER0_IRQ 0 /**< @brief Timer 0 IRQ line */
#define KBD_IRQ 1 /**< @brief Keyboard IRQ line */
#define MOUSE_IRQ 12 /**< @brief Mouse IRQ line */

/* I/O port addresses */

#define TIMER_0    0x40 /**< @brief Timer 0 count register */
#define TIMER_1    0x41 /**< @brief Timer 1 count register */
#define TIMER_2    0x42 /**< @brief Timer 2 count register */
#define TIMER_CTRL 0x43 /**< @brief Control register */

#define SPEAKER_CTRL 0x61 /**< @brief Register for speaker control  */

/* Timer control */

/* Timer selection: bits 7 and 6 */

#define TIMER_SEL0   0x00              /**< @brief Control Word for Timer 0 */
#define TIMER_SEL1   BIT(6)            /**< @brief Control Word for Timer 1 */
#define TIMER_SEL2   BIT(7)            /**< @brief Control Word for Timer 2 */
#define TIMER_RB_CMD (BIT(7) | BIT(6)) /**< @brief Read Back Command */

/* Register selection: bits 5 and 4 */

#define TIMER_LSB     BIT(4)                  /**< @brief Initialize Counter LSB only */
#define TIMER_MSB     BIT(5)                  /**< @brief Initialize Counter MSB only */
#define TIMER_LSB_MSB (TIMER_LSB | TIMER_MSB) /**< @brief Initialize LSB first and MSB afterwards */

/* Operating mode: bits 3, 2 and 1 */

#define TIMER_SQR_WAVE (BIT(2) | BIT(1)) /**< @brief Mode 3: square wave generator */
#define TIMER_RATE_GEN BIT(2)            /**< @brief Mode 2: rate generator */

/* Counting mode: bit 0 */

#define TIMER_BCD 0x01 /**< @brief Count in BCD */
#define TIMER_BIN 0x00 /**< @brief Count in binary */

/* READ-BACK COMMAND FORMAT */

#define TIMER_RB_COUNT_  BIT(5)
#define TIMER_RB_STATUS_ BIT(4)
#define TIMER_RB_SEL(n)  BIT((n) + 1)

/**@}*/


#define ESC_break   0x81  /**< @brief ESC key BREAK CODE value  */


#define OUT_BUF 0x60 /**< @brief OUT_BUF port */
#define STATUS_REG 0x64 /**< @brief STATUS_REG port */

#define OBF BIT(0) /**< @brief Bit for Output Buffer Full */
#define IBF BIT(1)
#define READ_CMD_BYTE 0x20 /**< @brief Read command byte */
#define WRITE_CMD_BYTE 0x60 /**< @brief Write command byte */

#define LARGEST_NUM 0xFF /**< @brief Largest number of 1 byte */
#define BYTE2_CODE 0xE0 /**< @brief code to read another byte from keyboard */

#define DELAY_US 2000 /**< @brief Delay US keyboard */


#define LB_BIT BIT(0) /**< @brief Bit of RB */
#define RB_BIT BIT(1) /**< @brief Bit of LB */
#define MB_BIT BIT(2) /**< @brief Bit of MB */
#define MSB_X_DELTA BIT(4)
#define MSB_Y_DELTA BIT(5)
#define X_OVF BIT(6)
#define Y_OVF BIT(7)

#define AUX BIT(5)

#define MS_WRITE_BYTE_CMD 0xD4

#define ENABLE_DATA_REPORTING 0xF4
#define DISABLE_DATA_REPORTING 0xF5

#define DISABLE_MOUSE 0xA7
#define ENABLE_MOUSE 0xA8

#define SET_DEFAULTS 0xF6

#define ENABLE_STREAM_MODE 0xEA

#define ACK 0xFA
#define NACK 0xFE
#define ERROR 0xFC

#define READ_MOUSE_DATA 0xEB
#define SET_RMT_MODE 0xF0
#define SET_STREAM_MODE 0xEA

#define MSB_INT16 0xFFFF


#endif /* _LCOM_I8254_H */
