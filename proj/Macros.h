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


#define ESC_break   0x81    /**< @brief ESC key BREAK CODE value  */
#define K1_break    0x82    /**< @brief 1 key BREAK CODE value  */
#define K2_break    0x83    /**< @brief 2 key BREAK CODE value  */
#define W_break     0x91    /**< @brief w key BREAK CODE value  */
#define A_break     0x9e    /**< @brief a key BREAK CODE value  */
#define S_break     0x9f    /**< @brief s key BREAK CODE value  */
#define D_break     0xa0    /**< @brief d key BREAK CODE value  */
#define ENTER_break 0x9c    /**< @brief ENTER key BREAK CODE value  */


#define OUT_BUF 0x60        /**< @brief OUT_BUF port */
#define STATUS_REG 0x64     /**< @brief STATUS_REG port */

#define OBF BIT(0)          /**< @brief Bit for Output Buffer Full */
#define IBF BIT(1)
#define READ_CMD_BYTE 0x20  /**< @brief Read command byte */
#define WRITE_CMD_BYTE 0x60 /**< @brief Write command byte */

#define LARGEST_NUM 0xFF    /**< @brief Largest number of 1 byte */
#define BYTE2_CODE 0xE0     /**< @brief code to read another byte from keyboard */

#define DELAY_US 2000       /**< @brief Delay US keyboard */


#define LB_BIT BIT(0)       /**< @brief Bit of RB */
#define RB_BIT BIT(1)       /**< @brief Bit of LB */
#define MB_BIT BIT(2)       /**< @brief Bit of MB */
#define MSB_X_DELTA BIT(4)  /**< @brief 9th bit of x delta */
#define MSB_Y_DELTA BIT(5)  /**< @brief 9th bit of y delta */
#define X_OVF BIT(6)        /**< @brief Bit of x overflow */
#define Y_OVF BIT(7)        /**< @brief Bit of y overflow */

#define AUX BIT(5)          /**< @brief Bit of mouse data */

#define MS_WRITE_BYTE_CMD 0xD4      /**< @brief Code of write byte to mouse */

#define ENABLE_DATA_REPORTING 0xF4  /**< @brief Mouse code to enable data reporting */
#define DISABLE_DATA_REPORTING 0xF5 /**< @brief Mouse code to disable data reporting */

#define ACK 0xFA    /**< @brief acknowledgment byte from mouse controller if everything OK */
#define NACK 0xFE   /**< @brief acknowledgment byte from mouse controller if invalid byte */
#define ERROR 0xFC  /**< @brief acknowledgment byte from mouse controller if second consecutive invalid byte*/

#define READ_MOUSE_DATA 0xEB  /**< @brief Send data packet request to mouse */
#define SET_STREAM_MODE 0xEA  /**< @brief Set mouse to stream mode */

#define MSB_INT16 0xFFFF      /**< @brief Largest number with 16 bits */


/*
---
SERIAL PORT
---
*/

// COM1
#define COM1_IRQ 4
#define COM1 0x3F8

// COM2
#define COM2_IRQ 3
#define COM2 0x2F8

// LCR
#define LCR_offset 3
#define DL_bitmask BIT(7)
#define SBE_bitmask BIT(6)

// LSR
#define LSR_offset 5


// Interrupt Enable Register
#define IER_offset 1
#define received_data_int_enable BIT(0)
#define transmitter_empty_int_enable BIT(1)
#define receiver_line_status_int_enable BIT(2)

// Interrupt Identification Register (with LCR DL disabled)
#define IIR_offset 2 // only read
#define IIR_int_status_bitmask BIT(0)
#define IIR_int_origin_bitmask (BIT(1)|BIT(2)|BIT(3))
#define RECEIVED_DATA BIT(1)
#define TRANSMITTER_EMPTY BIT(0) 
#define LINE_STATUS (BIT(0) | BIT(1))

// FIFO Control Register
#define FCR_offset 2 // write only
#define ENABLE_FIFO BIT(0)
#define FIFO_INT_TRIGGER_LVL_1 0
#define FIFO_INT_TRIGGER_LVL_4 BIT(6)
#define FIFO_INT_TRIGGER_LVL_8 BIT(7)
#define FIFO_INT_TRIGGER_LVL_14 BIT(6)|BIT(7)
#define CLEAR_RCV_FIFO BIT(1)
#define CLEAR_TRANSMIT_FIFO BIT(2)

// Divisor Latch (with LCR DL enabled)
#define DLL_offset 0
#define DLM_offset 1
#define DL_divisor 115200

// Bitrate
#define BR_1 1200
#define BR_2 2400
#define BR_3 4800
#define BR_4 9600
#define BR_5 19200
#define BR_6 38400
#define BR_7 57600
#define BR_8 115200

// Parity
#define PARITY_none 0x0
#define PARITY_odd 0x1
#define PARITY_even 0x3
#define PARITY_1 0x5
#define PARITY_0 0x7

// Word Length
#define WL_5 0x0
#define WL_6 0x1
#define WL_7 0x2
#define WL_8 0x3

/*
---
END serial port
---
*/


/*
---
GRAPHICS
---
*/

#define INDEXED_MODE 0x105

// Letters
#define N_LETTER_H 60       /**< @brief Normal letter height */
#define S_LETTER_H (N_LETTER_H/2)      /**< @brief Small letter height */
#define SR_LETTER_H (N_LETTER_H/3)      /**< @brief Smaller letter height */

//Colors
#define PREDEF_COLOR 0xFF0000
#define WHITE 0xFFFFFF
#define DIRTY_WHITE 0xC0C0C0
#define BLACK 0x000000

/*
---
END graphics
---
*/

//Game
#define BOARD_SIZE 17
#define PLAY_TIME 30
#define DEFAULT_BLOCK_COORDINATE -999

//Debug
//#define DEBUG



#endif /* _LCOM_I8254_H */
