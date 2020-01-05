#include <lcom/lcf.h>
#include <stdint.h>
#include <string.h>
#include "Macros.h"
#include "utils.h"
#include "queue.h"
#include "mouse.h"
#include "video.h"
#include "sp.h"
#include "keyboard.h"

int hook_id_com1 = 0, hook_id_com2 = 0;
int p1_mouse_xvariance = 400, p1_mouse_yvariance = 300;
bool p1_mouse_lb = false, received_player_data = false, opponent_quit = false;
uint8_t p1_kbd_code = 0;

extern bool host, host_has_been_set, sp_on, connected;
extern uint8_t irq_com1, irq_com2;

charqueue * transmission_queue = NULL;
charqueue * reception_queue = NULL;
extern charqueue * player1_name;
extern charqueue * player2_name;


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

uint8_t sp_get_lsr(unsigned base) {
  uint8_t lsr;

  util_sys_inb(base + LSR_offset, &lsr);

  return lsr;
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
  
  char * base_name;
  if (base == COM1) base_name = "COM1";
  else base_name = "COM2";

  printf("(%s) IER before: 0x%x\n", base_name, interrupt_enable_register);

  if (base == COM1) {
    if (host) {
      interrupt_enable_register = (transmitter_empty_int_enable | receiver_line_status_int_enable);
    } else {
      interrupt_enable_register = (received_data_int_enable | receiver_line_status_int_enable);
    }
  } else {
    if (host) {
      interrupt_enable_register = (received_data_int_enable | receiver_line_status_int_enable);
    } else {
      interrupt_enable_register = (transmitter_empty_int_enable | receiver_line_status_int_enable);
    }
  }

  printf("(%s) IER after: 0x%x\n", base_name, interrupt_enable_register);

  if (sys_outb(base + IER_offset, interrupt_enable_register) != 0) return 1;

  return 0;
}

int sp_disable_interrupts(unsigned base, bool host) {
  uint8_t interrupt_enable_register = 0, lcr = 0;

  // Making sure that DLAB is at 0
  if (util_sys_inb(base + LCR_offset, &lcr) != 0) return 1;
  if (lcr & DL_bitmask) {
    printf("DLAB wasn't 0\n");
    lcr &= ~DL_bitmask;
    if (sys_outb(base + LCR_offset, lcr) != 0) return 1;
  }


  if (util_sys_inb(base + 1, &interrupt_enable_register) != 0) return 1;
  
  char * base_name;
  if (base == COM1) base_name = "COM1";
  else base_name = "COM2";

  printf("(%s) IER before: 0x%x\n", base_name, interrupt_enable_register);

  if (base == COM1) {
    if (host) {
      interrupt_enable_register &= ~(transmitter_empty_int_enable | receiver_line_status_int_enable);
    } else {
      interrupt_enable_register &= ~(received_data_int_enable | receiver_line_status_int_enable);
    }
  } else {
    if (host) {
      interrupt_enable_register &= ~(received_data_int_enable | receiver_line_status_int_enable);
    } else {
      interrupt_enable_register &= ~(transmitter_empty_int_enable | receiver_line_status_int_enable);
    }
  }

  printf("(%s) IER after: 0x%x\n", base_name, interrupt_enable_register);

  if (sys_outb(base + IER_offset, interrupt_enable_register) != 0) return 1;

  return 0;
}


int sp_setup_fifo(unsigned base) {
  uint8_t fifo_control_register;

  fifo_control_register = ENABLE_FIFO | FIFO_INT_TRIGGER_LVL_1 | CLEAR_RCV_FIFO | CLEAR_TRANSMIT_FIFO;
  
  if (sys_outb(base + FCR_offset, fifo_control_register) != 0) return 0; 

  return 0;
}

int sp_disable_fifo(unsigned base) {
  return 0;
}

void sp_send_character(uint8_t c) {
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
  sp_set_conf(COM1, 8, 2, PARITY_even, BR_8);
  sp_set_conf(COM2, 8, 2, PARITY_even, BR_8);
  sp_print_conf(COM1);
  sp_print_conf(COM2);

  sp_on = true;
  com1_subscribe_int(&irq_com1);
  com2_subscribe_int(&irq_com2);

  printf("ENABLING INTERRUPTS\n");
  sp_enable_interrupts(COM1, host);
  sp_setup_fifo(COM1);

  sp_enable_interrupts(COM2, host);
  sp_setup_fifo(COM2);

  if (reception_queue != NULL) {
    free(reception_queue);
  }
  if (transmission_queue != NULL) {
    free(transmission_queue);
  }
  reception_queue = create_charqueue();
  transmission_queue = create_charqueue();
}

void sp_terminate() {
  printf("DISABLING INTERRUPTS\n");

  com1_unsubscribe_int();   // unsubscribes COM1 interrupts
  sp_disable_interrupts(COM1, host);

  com2_unsubscribe_int();
  sp_disable_interrupts(COM2, host);

  charqueue_make_empty(reception_queue);
  charqueue_make_empty(transmission_queue);
  free(reception_queue);
  free(transmission_queue);

  sp_on = false;
}

void transmit_mouse_bytes(struct packet * mouse_data, unsigned mouse_xvariance, unsigned mouse_yvariance) {
  if (charqueue_empty(transmission_queue)) {  // Checking if the transmitter is ready
    charqueue_push(transmission_queue, mouse_xvariance);
    charqueue_push(transmission_queue, mouse_xvariance >> 8);
    charqueue_push(transmission_queue, mouse_yvariance);
    charqueue_push(transmission_queue, mouse_yvariance >> 8);

    charqueue_push(transmission_queue, mouse_data->lb);
    
    charqueue_push_end_characters();  // end characters

    sp_send_character('M');  // indicates that the info being received belongs to mouse
  } else {
    charqueue_push(transmission_queue, 'M');

    charqueue_push(transmission_queue, mouse_xvariance);
    charqueue_push(transmission_queue, mouse_xvariance >> 8);
    charqueue_push(transmission_queue, mouse_yvariance);
    charqueue_push(transmission_queue, mouse_yvariance >> 8);

    charqueue_push(transmission_queue, mouse_data->lb);
    
    charqueue_push_end_characters();  // end characters
  }
}

void transmit_kbd_code(uint8_t kbd_code) {
  if (charqueue_empty(transmission_queue)) {  // Checking if the transmitter is ready
    charqueue_push(transmission_queue, kbd_code);
    
    charqueue_push_end_characters();  // end characters

    sp_send_character('K');  // indicates that the info being received belongs to keyboard
  } else {
    charqueue_push(transmission_queue, 'K');

    charqueue_push(transmission_queue, kbd_code);
    
    charqueue_push_end_characters();  // end characters
  }
}

void transmit_player_data() {
  char *str;
  unsigned str_len;
  if (host) {
    str = charqueue_to_string(player1_name);
  } else {
    str = charqueue_to_string(player2_name);
  }
  str_len = strlen(str);
  if (charqueue_empty(transmission_queue)) {  // Checking if the transmitter is ready
    
    charqueue_push(transmission_queue, str_len);

    for (unsigned i = 0; i < str_len; i++) {
      charqueue_push(transmission_queue, str[i]);
    }

    charqueue_push_end_characters();

    sp_send_character('P');
  } else {
    charqueue_push(transmission_queue, 'P');

    charqueue_push(transmission_queue, str_len);

    for (unsigned i = 0; i < str_len; i++) {
      charqueue_push(transmission_queue, str[i]);
    }
    
    charqueue_push_end_characters();
  }
  free(str);
}

void charqueue_push_end_characters() {
  for (unsigned i = 0; i < 3; i++) {
    charqueue_push(transmission_queue, '0');
  }
}

void transmit_string(char * str, uint8_t str_len) {
  if (charqueue_empty(transmission_queue)) {  // Checking if the transmitter is ready
    charqueue_push(transmission_queue, str_len);
    for (unsigned i = 0; i < str_len; i++) {
      charqueue_push(transmission_queue, str[i]);
    }
    charqueue_push_end_characters();

    sp_send_character('S');
  } else {
    charqueue_push(transmission_queue, 'S');
    charqueue_push(transmission_queue, str_len);
    for (unsigned i = 0; i < str_len; i++) {
      charqueue_push(transmission_queue, str[i]);
    }
    
    charqueue_push_end_characters();
  }
}

void transmit_critical_event(char * type) {
  uint8_t value;
  if (strcmp(type, "disconnect") == 0) {
    value = 'D';
  } else if (strcmp(type, "pause_game") == 0) {
    value = 'P';
  } else if (strcmp(type, "resume_game") == 0) {
    value = 'R';
  } else {
    printf("ERROR: Trying to transmit an unrecognisable critical event.\n");
    return;
  }
  if (charqueue_empty(transmission_queue)) {  // Checking if the transmitter is ready
    charqueue_push(transmission_queue, value);

    charqueue_push_end_characters();

    sp_send_character('C');
  } else {
    charqueue_push(transmission_queue, 'C');
    charqueue_push(transmission_queue, value);
    
    charqueue_push_end_characters();
  }
}

int retrieve_info_from_queue() {
  /*
    ret = 1 -> kbd code
    ret = 2 -> mouse code
    ret = 3 -> pause game
    ret = 4 -> resume game
  */
  int ret = 0;
  if (charqueue_front(reception_queue) == 'S') {  // Receiving a string
    charqueue_pop(reception_queue);

    unsigned char_no = charqueue_pop(reception_queue) + 1;

    char str[char_no];
    unsigned i = 0;

    while (!charqueue_empty(reception_queue) && i < (char_no-1)) {
      str[i] = charqueue_pop(reception_queue);
      i++;
    }
    str[i] = '\0';
    printf("Received string: %s\n", str);

  } else if (charqueue_front(reception_queue) == 'M') {  // Receiving a mouse packet

    charqueue_pop(reception_queue);

    p1_mouse_xvariance = charqueue_pop(reception_queue);
    p1_mouse_xvariance |= (charqueue_pop(reception_queue) << 8);
    p1_mouse_yvariance = charqueue_pop(reception_queue);
    p1_mouse_yvariance |= (charqueue_pop(reception_queue) << 8);
    p1_mouse_lb = charqueue_pop(reception_queue);

    ret = 2;

  } else if (charqueue_front(reception_queue) == 'K') {  // Receiving KBD code

    charqueue_pop(reception_queue);

    p1_kbd_code = charqueue_pop(reception_queue);

    ret = 1;
  } else if (!received_player_data && charqueue_front(reception_queue) == 'P') {   // Receiving player data
    charqueue_pop(reception_queue);

    unsigned char_no = charqueue_pop(reception_queue) + 1;

    char str[char_no];
    unsigned i = 0;

    while (!charqueue_empty(reception_queue) && i < (char_no-1)) {
      str[i] = charqueue_pop(reception_queue);
      i++;
    }
    str[i] = '\0';
    if (host) {
      for (unsigned i = 0; i < char_no-1; i++) {
        charqueue_push(player2_name, str[i]);
      }
    } else {
      for (unsigned i = 0; i < char_no-1; i++) {
        charqueue_push(player1_name, str[i]);
      }
    }
    received_player_data = true;
  } else if (charqueue_front(reception_queue) == 'C') {    // Receiving critical event
    charqueue_pop(reception_queue);

    uint8_t value = charqueue_pop(reception_queue);

    if (value == 'D') {
      opponent_quit = true;
    } else if (value == 'R') {
      ret = 4;
    } else if (value == 'P') {
      ret = 3;
    }
  }
  
  charqueue_make_empty(reception_queue);

  return ret;
}

unsigned zero_count = 0;

int sp_ih(unsigned com) {
  int ret = 0;
  uint8_t iir;
  uint8_t c = 0;
  
  unsigned com_no;
  if (com == COM2) com_no = 2; else com_no = 1;

  util_sys_inb(com + IIR_offset, &iir);

  if(!(iir & IIR_int_status_bitmask)) {
    switch( (iir & (IIR_int_origin_bitmask)) >> 1 ) {
      case RECEIVED_DATA: case RECEIVED_DATA_1:  // From extensive testing we realised that received data could be either 010 or 110
        //printf("\n-- COM%d: Received data - %c\n", com_no, c);
        if ((host && (com == COM2)) || (!host && (com == COM1))) {
          util_sys_inb(com, &c);
          if (!connected) {
            // This is the syncing process
            if (c == 'a') {
              //printf("Received an 'a'\n");
              sp_send_character('b');
              connected = true;
            } else if (c == 'b') {
              //printf("Received a 'b'\n");
              connected = true;
            } else {
              printf("\n-- COM%d: Received data - %c\n", com_no, c);
            }
          } else {
            //printf("\n-- COM%d: Received data - %c\n", com_no, c);
            charqueue_push(reception_queue, c);
            if (c == '0') {  // '0' is the char that represents that a piece of information has been fully received
              zero_count++;
              if (zero_count == 3) {
                zero_count = 0;
                ret = retrieve_info_from_queue();
              }
            } else {
              zero_count = 0;
            }
          }
        }
        break;
      case TRANSMITTER_EMPTY:
        //printf("\n-- COM%d: Transmitter empty\n", com_no);
        if ((host && (com == COM1)) || (!host && (com == COM2))) {
          if (connected) {
            if (!charqueue_empty(transmission_queue)) {
              //printf("Sending %c\n", charqueue_front(transmission_queue));
              sp_send_character(charqueue_pop(transmission_queue)); // pops the character from the queue and sends it
            }
          }
        }
        break;
      case LINE_STATUS:
        printf("COM%d: Line status interrupt. ", com_no);
        sp_print_lsr(com);
        uint8_t c = 0;
        util_sys_inb(com, &c);
        printf("COM%d had %x in it's buffer.\n", com_no, c);
        break;
      default:
        printf("COM%d IH: Received an interrupt I can't handle :(\nIIR: 0x%x, ", com_no, iir);
        sp_print_lsr(com);
        break;
    }
  }
  return ret;
}

