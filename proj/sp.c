#include <lcom/lcf.h>

#include <stdint.h>

#include "Macros.h"
#include "utils.h"

int hook_id_sp = 0;

int (com1_subscribe_int)(uint8_t *bit_no){
  hook_id_sp = (int) *bit_no; // saves bit_no value
  
  // subscribe a notification on interrupts
  if (sys_irqsetpolicy(COM1_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &hook_id_sp) != 0)
    return 1;
  
  *bit_no = (uint8_t) BIT(*bit_no);

  return 0;
}

int (com1_unsubscribe_int)() {
  // removes notification subscriptions on interrupts
  if (sys_irqrmpolicy(&hook_id_sp) != 0)
    return 1;

  return 0;
}

int sp_get_divisor_latch(unsigned base, unsigned *dl) {
  uint8_t lcr = 0, msb = 0, lsb = 0, prev_lcr;

  if (util_sys_inb(base + LCR_offset, &lcr) != 0) return 1;

  prev_lcr = lcr;
  lcr |= DL_bitmask;

  if (sys_outb(base + LCR_offset, lcr) != 0) return 1;

  if (util_sys_inb(base + DLL_offset, &lsb) != 0) return 1;

  if (util_sys_inb(base + DLM_offset, &msb) != 0) return 1;

  if (sys_outb(base + LCR_offset, prev_lcr) != 0) return 1;

  *dl = (msb << 8) + lsb;

  return 0;
}

int sp_set_bitrate(unsigned base, unsigned bitrate) {
  unsigned dl;
  uint8_t lcr = 0, msb, lsb, prev_lcr;

  dl = DL_divisor / bitrate;

  if (util_sys_inb(base + LCR_offset, &lcr) != 0) return 1;

  prev_lcr = lcr;
  lcr |= DL_bitmask;

  if (sys_outb(base + LCR_offset, lcr) != 0) return 1;

  msb = (uint8_t) dl >> 8;
  lsb = (uint8_t) dl & 0xFF;

  if (sys_outb(base + DLM_offset, msb) != 0) return 1;

  if (sys_outb(base + DLL_offset, lsb) != 0) return 1;

  if (sys_outb(base + LCR_offset, prev_lcr) != 0) return 1;

  return 0;
}

int sp_print_conf(unsigned base) {
  uint8_t lcr = 0;
  unsigned dl;

  if (sp_get_divisor_latch(base, &dl) != 0 ) return 1;

  if (util_sys_inb(base + LCR_offset, &lcr) != 0) return 1;

  printf("\n- SERIAL PORT CONNECTION (0x%x)\n", base);
  printf("LCR: %x\n", lcr);
  printf("Bitrate: %d\n", DL_divisor / dl);
  printf("Word length: %d\n", (lcr & 0x03) + 5);
  printf("Stop bits: %d\n", ((lcr & 0x04) >> 2) + 1);


  unsigned par = (lcr & 0x38) >> 3;
  switch (par) {
    case PARITY_odd:
      printf("Parity: odd\n");
      break;
    case PARITY_even:
      printf("Parity: even\n");
      break;
    case PARITY_1:
      printf("Parity: 1\n");
      break;
    case PARITY_0:
      printf("Parity: 0\n");
      break;
    default:
      if (((lcr & 0x08) >> 3) == 0) {
        printf("Parity: none\n");
      }
      break;
  }
  
  return 0;
}

int sp_print_lsr(unsigned base) {
  uint8_t lsr;

  if (util_sys_inb(base + LSR_offset, &lsr) != 0) return 1;

  printf("LSR: %x\n", lsr);

  return 0;
}

int sp_set_conf(unsigned base, unsigned word_length, unsigned stop_bits, unsigned parity, unsigned bitrate) {
  uint8_t lcr = 0;

  if (word_length < 5 || word_length > 8) {
    printf("invalid word_length\n");
    return 1;
  }

  if (stop_bits < 1 || stop_bits > 2) {
    printf("invalid stop_bits\n");
    return 1;
  }

  if (parity != PARITY_none && parity != PARITY_odd && parity != PARITY_even && parity != PARITY_1 && parity != PARITY_0) {
    printf("invalid parity\n");
    return 1;
  }

  /*
  Bit rates commonly supported include 75, 110, 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600 and 115200 bit/s.
  We don't use the 75, 110, 300 because it would be too low, even if it is supported.
  */

  if (bitrate != BR_1 && bitrate != BR_2 && bitrate != BR_3 && bitrate != BR_4 && bitrate != BR_5 && bitrate != BR_6 && bitrate != BR_7 && bitrate != BR_8) {
    printf("invalid bitrate");
    return 1;
  }

  if (util_sys_inb(base + LCR_offset, &lcr) != 0) return 1;

  switch (word_length) {
    case 5:
      word_length = WL_5;
      break;
    case 6:
      word_length = WL_6;
      break;
    case 7:
      word_length = WL_7;
      break;
    case 8:
      word_length = WL_8;
      break;
    default:
      break;
  }

  lcr = (lcr & (DL_bitmask|SBE_bitmask)) | word_length | ((stop_bits-1) << 2) | (parity << 3);

  if (sys_outb(base + LCR_offset, lcr) != 0) return 1;

  if (sp_set_bitrate(base, bitrate) != 0) return 1;

  return 0;
}

int sp_enable_interrupts(unsigned base) {
  uint8_t interrupt_enable_register = 0, lcr = 0;

  // Making sure that DLAB is at 0
  if (util_sys_inb(base + LCR_offset, &lcr) != 0) return 1;
  if (lcr & DL_bitmask) {
    printf("DLAB wasn't 0\n");
    lcr &= ~DL_bitmask;
    if (sys_outb(base + LCR_offset, lcr) != 0) return 1;
  }


  if (util_sys_inb(base + 1, &interrupt_enable_register) != 0) return 1;
  
  printf("IER: 0x%x\n", interrupt_enable_register);

  interrupt_enable_register = received_data_int_enable | transmitter_empty_int_enable | receiver_line_status_int_enable;

  printf("IER: 0x%x\n", interrupt_enable_register);

  if (sys_outb(base + IER_offset, interrupt_enable_register) != 0) return 1;

  return 0;
}

int sp_setup_fifo(unsigned base) {
  uint8_t fifo_control_register;

  fifo_control_register = ENABLE_FIFO | FIFO_INT_TRIGGER_LVL_1 | CLEAR_RCV_FIFO | CLEAR_TRANSMIT_FIFO;
  
  if (sys_outb(base + FCR_offset, fifo_control_register) != 0) return 0; 

  return 0;
}

void com1_ih() {
  uint8_t iir;

  util_sys_inb(COM1 + IIR_offset, &iir);

  if(! (iir & IIR_int_status_bitmask) ) {
    switch( (iir & (IIR_int_origin_bitmask)) >> 1 ) {
      case RECEIVED_DATA:
        printf("COM1: Received data\n");
        break; /* read received character */
      case TRANSMITTER_EMPTY:
        printf("COM1: Transmitter empty\n");
        break; /* put character to sent */
      case LINE_STATUS:
        printf("COM1: Line status\n");
        break; /* notify upper level */
      default:
        printf("COM1 IH: Received an interrupt I can't handle :(\n");
        break;
    }
  }
}

