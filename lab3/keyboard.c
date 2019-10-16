#include <lcom/lcf.h>

#include <stdint.h>

#include "Macros.h"


int hook_id = 0;
unsigned int sys_inb_counter = 0;
uint8_t kbd_code;


int (kbd_subscribe_int)(uint8_t *bit_no) {
  hook_id = (int) *bit_no; // saves bit_no value
  
  // subscribe a notification on interrupts
  if (sys_irqsetpolicy(KBD_IRQ,IRQ_REENABLE|IRQ_EXCLUSIVE,&hook_id) != 0)
    return 1;
  
  *bit_no = (uint8_t) BIT(*bit_no);

  return 0;
}

int (kbd_unsubscribe_int)() {
  // removes notification subscriptions on interrupts
  if (sys_irqrmpolicy(&hook_id) != 0)
    return 1;

  return 0;
}


void enable_interrupts(){
  uint8_t command = 0;
  
  sys_outb(STATUS_REG, READ_CMD_BYTE);

  util_sys_inb(OUT_BUF, &command);

  sys_outb(STATUS_REG, WRITE_CMD_BYTE);

  command = command | BIT(0);

  sys_outb(OUT_BUF,command);
}
