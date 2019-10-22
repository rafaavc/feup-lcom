#include <lcom/lcf.h>

#include <stdint.h>

#include "Macros.h"


int hook_id_kbd = 0;
unsigned int sys_inb_counter = 0;
uint8_t kbd_code;

int hook_id_timer = TIMER_SEL0;

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
