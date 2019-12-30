#include <lcom/lcf.h>

#include <stdint.h>

#include "Macros.h"
#include "utils.h"

int hook_id_com1 = 0, hook_id_com2 = 0;
extern bool host, host_has_been_set, sp_on, connected;
extern uint8_t irq_com1, irq_com2;

int (com1_subscribe_int)(uint8_t *bit_no){
  hook_id_com1 = (int) *bit_no; // saves bit_no value
  
  // subscribe a notification on interrupts
  if (sys_irqsetpolicy(COM1_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &hook_id_com1) != 0)
    return 1;
  
  *bit_no = (uint8_t) BIT(*bit_no);

  return 0;
}

int (com1_unsubscribe_int)() {
  // removes notification subscriptions on interrupts
  if (sys_irqrmpolicy(&hook_id_com1) != 0)
    return 1;

  return 0;
}

int (com2_subscribe_int)(uint8_t *bit_no){
  hook_id_com2 = (int) *bit_no; // saves bit_no value
  
  // subscribe a notification on interrupts
  if (sys_irqsetpolicy(COM2_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &hook_id_com2) != 0)
    return 1;
  
  *bit_no = (uint8_t) BIT(*bit_no);

  return 0;
}

int (com2_unsubscribe_int)() {
  // removes notification subscriptions on interrupts
  if (sys_irqrmpolicy(&hook_id_com2) != 0)
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

int sp_enable_interrupts(unsigned base, bool host) {
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

  if (base == COM1) {
    if (host) {
      interrupt_enable_register = transmitter_empty_int_enable | receiver_line_status_int_enable;
    } else {
      interrupt_enable_register = received_data_int_enable | receiver_line_status_int_enable;
    }
  } else {
    if (host) {
      interrupt_enable_register = received_data_int_enable | receiver_line_status_int_enable;
    } else {
      interrupt_enable_register = transmitter_empty_int_enable | receiver_line_status_int_enable;
    }
  }

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

void sp_send_character(char c, bool reverse) {
  if (reverse) {
    if (host) {
      sys_outb(COM2, c);
      //printf("sent %c to COM2\n", c);
    } else {
      sys_outb(COM1, c);
      //printf("sent %c to COM1\n", c);
    }
    return;
  }
  // This is what will happen in most cases: 
  if (host) {
    sys_outb(COM1, c);
    //printf("sent %c to COM1\n", c);
  } else {
    sys_outb(COM2, c);
    //printf("sent %c to COM2\n", c);
  }
}

void sp_init() {
  sp_set_conf(COM1, 8, 2, PARITY_even, BR_4);
  sp_set_conf(COM2, 8, 2, PARITY_even, BR_4);
  sp_print_conf(COM1);
  sp_print_conf(COM2);

  sp_on = true;
  com1_subscribe_int(&irq_com1);
  com2_subscribe_int(&irq_com2);

  printf("COM1");
  sp_enable_interrupts(COM1, host);
  sp_setup_fifo(COM1);

  printf("COM1");
  sp_enable_interrupts(COM2, host);
  sp_setup_fifo(COM2);
}

void sp_terminate() {
  com1_unsubscribe_int();   // unsubscribes COM1 interrupts
  com2_unsubscribe_int();
  sp_on = false;
}

int count = 3;

void sp_ih(unsigned com, unsigned com_no) {
  uint8_t iir;
  uint8_t c;

  util_sys_inb(com + IIR_offset, &iir);

  if(! (iir & IIR_int_status_bitmask) ) {
    switch( (iir & (IIR_int_origin_bitmask)) >> 1 ) {
      case RECEIVED_DATA:
        util_sys_inb(com, &c);
        if (!connected) {
          // This is the syncing process
          if (c == 'a') {
            //printf("Received an 'a'\n");
            sp_send_character('b', false);
            connected = true;
          } else if (c == 'b') {
            //printf("Received a 'b'\n");
            connected = true;
          } else {
            printf("\n-- COM%d: Received data - %c\n", com_no, c);
          }
        } else {
          printf("\n-- COM%d: Received data - %c\n", com_no, c);
        }

        // This line makes sure we receive the next received data interrupt
        sp_send_character('b', true);
        break;
      case TRANSMITTER_EMPTY:
        printf("\n-- COM%d: Transmitter empty\n", com_no);
        /*if (count > 0) {
          sp_send_character('a', false);
          count--;
        }*//* else {
          count--;
        }
        if (count == -1) {
          sp_send_character('b', true);
          count --;
        }*/
        break;
      case LINE_STATUS:
        printf("COM%d: Line status interrupt. ", com_no);
        sp_print_lsr(com);
        break;
      default:
        printf("COM%d IH: Received an interrupt I can't handle :(\nIIR: 0x%x, ", com_no, iir);
        sp_print_lsr(com);
        break;
    }
  }
}

