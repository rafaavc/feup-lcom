#include <lcom/lcf.h>

#include <stdint.h>

#include "Macros.h"

int hook_id_mouse = 0;
uint8_t mouse_code;
bool error = false;
uint8_t bytes_read[3];

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

  uint8_t status_reg_content;

  if (util_sys_inb(STATUS_REG, &status_reg_content) != 0) error = true;

  if (status_reg_content & (BIT(6) | BIT(7))) error = true;

  if ((status_reg_content & OBF) && !error && (status_reg_content & BIT(5))) {
    
    if (sys_outb(OUT_BUF, mouse_code) != 0) error = true;

  } else { error = true; }

} 

int mouse_polling(uint16_t period){
  uint8_t status_reg_content = 0;
  
  if (util_sys_inb(STATUS_REG, &status_reg_content) != 0) return 1;

  if (status_reg_content & (BIT(7) | BIT(6))) return 1;
  uint8_t n = 0;
  while (true) {
    if (status_reg_content & OBF && status_reg_content & BIT(5)){

      if (util_sys_inb(OUT_BUF, &mouse_code) != 0) return 1;

    }
    bytes_read[n] = mouse_code;
    if (n == 2) break;
    n++;
    tickdelay(micros_to_ticks(DELAY_US));

  }
  return 0;
}
