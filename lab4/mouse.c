#include <lcom/lcf.h>

#include <stdint.h>

#include "Macros.h"

int hook_id_mouse = 0;
uint8_t mouse_code;

int (mouse_subscribe_int)(uint8_t *bit_no){
  hook_id_mouse = (int) *bit_no; // saves bit_no value
  
  // subscribe a notification on interrupts
  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &hook_id_mouse) != 0)
    return 1;
  
  *bit_no = (uint8_t) BIT(*bit_no);

  return 0;
}

int mouse_enable_data_recording() {
  return 0;
}


int (mouse_unsubscribe_int)() {
  // removes notification subscriptions on interrupts
  if (sys_irqrmpolicy(&hook_id_mouse) != 0)
    return 1;

  return 0;
}

void (mouse_ih)(void) {
  uint8_t status_data;
  bool error = false;

  if (util_sys_inb(STATUS_REG, &status_data) != 0) error = true;  // Reads the Status Reg

  if (status_data & (BIT(7) | BIT(6) | !BIT(5))) error = true; // Checks parity error, Timeout error and Mouse data

  if (status_data & BIT(0)){ // Makes sure that Output buffer is full
    if (util_sys_inb(OUT_BUF, &mouse_code) != 0) error = true;
  }

  if (mouse_code == LARGEST_NUM)
    mouse_code = 0;
  if (error)
    mouse_code = 0;
}
