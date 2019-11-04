#include <lcom/lcf.h>

#include <stdint.h>

#include "Macros.h"

int hook_id_mouse = 0;
uint8_t mouse_code;
bool error = false;

int (mouse_subscribe_int)(uint8_t *bit_no){
  hook_id_mouse = (int) *bit_no; // saves bit_no value
  
  // subscribe a notification on interrupts
  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &hook_id_mouse) != 0)
    return 1;
  
  *bit_no = (uint8_t) BIT(*bit_no);

  return 0;
}


int send_statusreg_command(uint8_t cmd, uint8_t args){
  uint8_t status_reg_content = 0, response = 0;
  if (util_sys_inb(STATUS_REG, &status_reg_content) != 0) return 1;

  if ((status_reg_content & IBF) != 0) return 1;

  if (sys_outb(STATUS_REG, cmd) != 0) return 1;

  if (args != 0){
    if (sys_outb(OUT_BUF, args) != 0) return 1;

    if (util_sys_inb(OUT_BUF, &response) != 0) return 1;

    if (response == NACK || response == ERROR) return 1;
  }

  return 0;
}

int mouse_enable_data_reporting_original() {
  return 0;
}

int mouse_disable_data_reporting() {
  return 0;
}


int (mouse_unsubscribe_int)() {
  // removes notification subscriptions on interrupts
  if (sys_irqrmpolicy(&hook_id_mouse) != 0)
    return 1;

  return 0;
}

void (mouse_ih)(void) {
  
} 

int mouse_polling(){
  
  return 0;
}
