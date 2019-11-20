#include <lcom/lcf.h>

#include <stdint.h>

#include "Macros.h"


int hook_id_kbd = 0;
unsigned int sys_inb_counter = 0;
uint8_t kbd_code, timer_counter = 0;

int hook_id_timer = TIMER_SEL0;

void (timer_int_handler)() {
  timer_counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {

  *st = (TIMER_RB_CMD|TIMER_RB_COUNT_|TIMER_RB_SEL(timer)); // read-back command

  if (sys_outb(TIMER_CTRL, *st) != 0) // writing to control
    return 1;
 
  if (timer == 0) {
    if (util_sys_inb(TIMER_0, st) != 0) // getting conf from timer
      return 1;
  } else if (timer == 1) {
    if (util_sys_inb(TIMER_1, st) != 0)
      return 1;
  } else if (timer == 2) {
    if (util_sys_inb(TIMER_2, st) != 0)
      return 1;
  } else {
    return 1;
  }
  return 0;
}

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  if (freq < 19 || freq > TIMER_FREQ)
    return 1;

  uint8_t current_conf;
  printf("1\n");
  fflush(stdout);
  if (timer_get_conf(timer, &current_conf) != 0) // gets current timer conf so that the 4 least significant bits aren't changed
    return 1;
  printf("1\n");
  fflush(stdout);
  uint8_t control_word = current_conf << 4;
  control_word = control_word >> 4;  // now control_word holds the 4 least significant bits (BCD/binary and operating mode)

  control_word |= TIMER_LSB_MSB;  // initializes lsb followed by msb
  uint8_t lsb, msb;
  uint16_t count_value = TIMER_FREQ / freq;

  util_get_LSB(count_value, &lsb);
  util_get_MSB(count_value, &msb);
  printf("1\n");
  fflush(stdout);
  switch (timer) {
    case 0:
      control_word |= TIMER_SEL0;
      if (sys_outb(TIMER_CTRL, control_word) != 0) // writing to control
        return 1;
      if (sys_outb(TIMER_0, lsb) != 0) // giving lsb
        return 1;
      if (sys_outb(TIMER_0, msb) != 0) // and now msb
        return 1;
      break;
    case 1:
      control_word |= TIMER_SEL1;
      if (sys_outb(TIMER_CTRL, control_word) != 0)
        return 1;
      if (sys_outb(TIMER_1, lsb) != 0)
        return 1;
      if (sys_outb(TIMER_1, msb) != 0)
        return 1;
      break;
    case 2:
      control_word |= TIMER_SEL2;
      if (sys_outb(TIMER_CTRL, control_word) != 0)
        return 1;
      if (sys_outb(TIMER_2, lsb) != 0)
        return 1;
      if (sys_outb(TIMER_2, msb) != 0)
        return 1;
      break;
    default:
      return 1;
  }
  printf("1\n");
  fflush(stdout);
  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  hook_id_timer = (int) *bit_no; // saves bit_no value
  
  // subscribe a notification on interrupts
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id_timer) != 0)
    return 1;
  
  *bit_no = (uint8_t) BIT(*bit_no);

  return 0;
}

int (timer_unsubscribe_int)() {
  // removes notification subscriptions on interrupts
  if (sys_irqrmpolicy(&hook_id_timer) != 0)
    return 1;

  return 0;
}

int (kbd_subscribe_int)(uint8_t *bit_no) {
  hook_id_kbd = (int) *bit_no; // saves bit_no value
  
  // subscribe a notification on interrupts
  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &hook_id_kbd) != 0)
    return 1;
  
  *bit_no = (uint8_t) BIT(*bit_no);

  return 0;
}

int (kbd_unsubscribe_int)() {
  // removes notification subscriptions on interrupts
  if (sys_irqrmpolicy(&hook_id_kbd) != 0)
    return 1;

  return 0;
}

int send_statusreg_commandbyte(uint8_t mask_enable, uint8_t mask_disable){
  /*
  This function receives a mask_enable, that will enable bits in the command byte and a mask_disable, that will disable bits in the command byte
  */
  uint8_t command = 0;
  uint8_t status_reg_content = 0;
  if (util_sys_inb(STATUS_REG, &status_reg_content) != 0) return 1;  // Reads status register

  if ((status_reg_content & BIT(1)) == 0) {    //  Makes sure that input buffer isn't full
    sys_outb(STATUS_REG, READ_CMD_BYTE);      //  Sends instruction to status register to read command byte, goes to outbuffer
  } else {
    return 1;
  }

  if (util_sys_inb(OUT_BUF, &command) != 0) return 1;    // reads command byte from output buffer

  if (mask_enable != 0)
    command = command | mask_enable;  // Enables bits that are 1 in mask of command
  if (mask_disable != 0)
    command = command & mask_disable;   // Disables bits that are 0 in mask of command

  if (sys_outb(STATUS_REG, WRITE_CMD_BYTE) != 0) return 1;    // sends information that command byte will be written    
  if (sys_outb(OUT_BUF, command) != 0) return 1;    // sends command byte through outbuffer
  return 0;
}


int enable_interrupts(){
  if (send_statusreg_commandbyte(BIT(0), 0) != 0) return 1;  // Bit 0 enables interrupts from keyboard ("INT: enable interrupt on OBF, from keyboard")
  return 0;
}

