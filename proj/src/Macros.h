#ifndef MACROS_H_
#define MACROS_H_

#include <lcom/lcf.h>

/** @defgroup i8254 i8254
 * @{
 *
 * Constants the macros used in the program
 */

#define TIMER_FREQ 1193182 /**< @brief clock frequency for timer in PC and AT */
#define TIMER0_IRQ 0 /**< @brief Timer 0 IRQ line */
#define KBD_IRQ 1 /**< @brief Keyboard IRQ line */
#define MOUSE_IRQ 12 /**< @brief Mouse IRQ line */
#define RTC_IRQ 8 /**< @brief RTC IRQ line */

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

#define TIMER_RB_COUNT_  BIT(5)         /**< @brief read back count bitmask */
#define TIMER_RB_STATUS_ BIT(4)         /**< @brief read back status bitmask */
#define TIMER_RB_SEL(n)  BIT((n) + 1)   /**< @brief read back select bitmask */

/**@}*/


#define ESC_break   0x81      /**< @brief ESC key BREAK CODE value  */
#define K1_break    0x82      /**< @brief 1 key BREAK CODE value  */
#define K2_break    0x83      /**< @brief 2 key BREAK CODE value  */

#define W_break     0x91      /**< @brief w key BREAK CODE value  */
#define A_break     0x9e      /**< @brief a key BREAK CODE value  */
#define S_break     0x9f      /**< @brief s key BREAK CODE value  */
#define D_break     0xa0      /**< @brief d key BREAK CODE value  */

#define B_break     0xb0      /**< @brief b key BREAK CODE value  */
#define C_break     0xae      /**< @brief c key BREAK CODE value  */
#define E_break     0x92      /**< @brief e key BREAK CODE value  */
#define F_break     0xa1      /**< @brief f key BREAK CODE value  */
#define G_break     0xa2      /**< @brief g key BREAK CODE value  */
#define H_break     0xa3      /**< @brief h key BREAK CODE value  */
#define I_break     0x97      /**< @brief i key BREAK CODE value  */
#define J_break     0xa4      /**< @brief j key BREAK CODE value  */
#define K_break     0xa5      /**< @brief k key BREAK CODE value  */
#define L_break     0xa6      /**< @brief l key BREAK CODE value  */
#define M_break     0xb2      /**< @brief m key BREAK CODE value  */
#define N_break     0xb1      /**< @brief n key BREAK CODE value  */
#define O_break     0x98      /**< @brief o key BREAK CODE value  */
#define P_break     0x99      /**< @brief p key BREAK CODE value  */
#define Q_break     0x90      /**< @brief q key BREAK CODE value  */
#define R_break     0x93      /**< @brief r key BREAK CODE value  */
#define T_break     0x94      /**< @brief t key BREAK CODE value  */
#define U_break     0x96      /**< @brief u key BREAK CODE value  */
#define V_break     0xaf      /**< @brief v key BREAK CODE value  */
#define X_break     0xad      /**< @brief x key BREAK CODE value  */
#define Y_break     0x95      /**< @brief y key BREAK CODE value  */
#define Z_break     0xac      /**< @brief z key BREAK CODE value  */

#define ENTER_break 0x9c      /**< @brief ENTER key BREAK CODE value  */
#define BACKSPACE_break 0x8e  /**< @brief BACKSPACE key BREAK CODE value  */


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
#define COM1_IRQ 4     /**< @brief COM1 IRQ */
#define COM1 0x3F8     /**< @brief COM1 address */

// COM2
#define COM2_IRQ 3      /**< @brief COM2 IRQ */
#define COM2 0x2F8      /**< @brief COM2 address */

// LCR
#define LCR_offset 3        /**< @brief offset of line control register (LCR) */
#define DL_bitmask BIT(7)   /**< @brief bitmask of DLAB */
#define SBE_bitmask BIT(6)  /**< @brief bitmask of set break enable (SBE) */

// LSR
#define LSR_offset 5    /**< @brief offset of line status register (LSR) */


// Interrupt Enable Register
#define IER_offset 1                              /**< @brief offset of interrupt enable register (IER) */
#define received_data_int_enable BIT(0)           /**< @brief bitmask of received data interruption */
#define transmitter_empty_int_enable BIT(1)       /**< @brief bitmask of transmitter empty interruption */
#define receiver_line_status_int_enable BIT(2)    /**< @brief bitmask of line status interruption*/

// Interrupt Identification Register (with LCR DL disabled)
#define IIR_offset 2 // only read                       /**< @brief offset do interrupt identification register (IIR) */
#define IIR_int_status_bitmask BIT(0)                   /**< @brief bitmask for interrupt status*/
#define IIR_int_origin_bitmask (BIT(1)|BIT(2)|BIT(3))   /**< @brief bitmask for interrupt origin */
#define RECEIVED_DATA BIT(1)                            /**< @brief bitmask for received data in IIR */
#define RECEIVED_DATA_1 (BIT(2)|BIT(1))  /**< @brief From extensive testing we realised that received data could be either 010 or 110 */
#define TRANSMITTER_EMPTY BIT(0)         /**< @brief bitmask for transmitter empty */
#define LINE_STATUS (BIT(0) | BIT(1))    /**< @brief bitmask for line status */

// FIFO Control Register
#define FCR_offset 2 // write only                  /**< @brief offset of fifo contro register (FCR) */
#define ENABLE_FIFO BIT(0)                          /**< @brief enable fifo bitmask */
#define FIFO_INT_TRIGGER_LVL_1 0                    /**< @brief fifo interruption trigger level 1 bitmask */
#define FIFO_INT_TRIGGER_LVL_4 BIT(6)               /**< @brief fifo interruption trigger level 4 bitmask */
#define FIFO_INT_TRIGGER_LVL_8 BIT(7)               /**< @brief fifo interruption trigger level 8 bitmask */
#define FIFO_INT_TRIGGER_LVL_14 BIT(6)|BIT(7)       /**< @brief fifo interruption trigger level 14 bitmask */
#define CLEAR_RCV_FIFO BIT(1)                       /**< @brief bitmask of clear receive fifo */
#define CLEAR_TRANSMIT_FIFO BIT(2)                  /**< @brief bitmask of clear transmit fifo */

// Divisor Latch (with LCR DL enabled)
#define DLL_offset 0        /**< @brief offset of divisor latch least significant byte (DLL) */
#define DLM_offset 1        /**< @brief offset of divisor latch most significant byte (DLM) */
#define DL_divisor 115200   /**< @brief DL divisor */

// Bitrate
#define BR_1 1200       /**< @brief possible bitrate 1 */
#define BR_2 2400       /**< @brief possible bitrate 2 */
#define BR_3 4800       /**< @brief possible bitrate 3 */
#define BR_4 9600       /**< @brief possible bitrate 4 */
#define BR_5 19200      /**< @brief possible bitrate 5 */
#define BR_6 38400      /**< @brief possible bitrate 6 */
#define BR_7 57600      /**< @brief possible bitrate 7 */
#define BR_8 115200     /**< @brief possible bitrate 8 */

// Parity
#define PARITY_none 0x0     /**< @brief no parity bitmask */
#define PARITY_odd 0x1      /**< @brief odd parity bitmask */
#define PARITY_even 0x3     /**< @brief even parity bitmask */
#define PARITY_1 0x5        /**< @brief parity 1 bitmask */
#define PARITY_0 0x7        /**< @brief parity 0 bitmask */

// Word Length
#define WL_5 0x0        /**< @brief word length 5 bitmask */
#define WL_6 0x1        /**< @brief word length 6 bitmask */
#define WL_7 0x2        /**< @brief word length 7 bitmask */
#define WL_8 0x3        /**< @brief word length 8 bitmask */

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
#define PREDEF_COLOR 0xFF0000   /**< @brief Predefined color (transparent) */
#define WHITE 0xFFFFFF          /**< @brief white color */
#define DIRTY_WHITE 0xC0C0C0    /**< @brief dirty white color */
#define BLACK 0x000000          /**< @brief black color */
#define LESS_BLACK 0x333333     /**< @brief less black color */

/*
---
END graphics
---
*/

//Game
#define BOARD_SIZE 17           /**< @brief board size */
#define PLAY_TIME 10            /**< @brief seconds to move the ball */
#define PLAY_TIME_MOVING_BLOCKS 30      /**< @brief seconds to move the blocks */
#define DEFAULT_BLOCK_COORDINATE -999   /**< @brief default block coordinate */
#define MAX_NAME_SIZE 15                /**< @brief maximum player name size */

//Debug
//#define DEBUG

/*
---
REAL TIME CLOCK
---
*/

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71

#define REG_A 0xA
#define REG_B 0xB
#define REG_C 0xC

#define REG_DARK_MODE 0xF       // can be auto ('A') or custom ('C')
#define REG_DARK_MODE1 0x1F      // can be true ('T') or false ('F')

#define SECONDS 0x0
#define MINUTES 0x2
#define HOURS 0x4

#define UPDATE_IN_PROGRESS BIT(7)
#define FORMAT_24 BIT(1)
#define RTC_BINARY BIT(2)
#define UPDATE_INTERRUPTS BIT(4)
#define PERIODIC_INTERRUPTS BIT(6)
#define ALARM_INTERRUPTS BIT(5)

#define C_UPDATE_FLAG BIT(4)
#define C_ALARM_FLAG BIT(5)
#define C_PERIODIC_FLAG BIT(6)

/*
---
END real time clock
---
*/

/** @}*/

#endif /* _LCOM_I8254_H */
