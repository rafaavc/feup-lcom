#include <lcom/lcf.h>

#include <stdint.h>

#include "Macros.h"

int hook_id_mouse = 0;
uint8_t mouse_code;
uint8_t bytes_read[3];

int (mouse_subscribe_int)(uint8_t *bit_no){
  hook_id_mouse = (int) *bit_no; // saves bit_no value
  
  // subscribe a notification on interrupts
  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &hook_id_mouse) != 0)
    return 1;
  
  *bit_no = (uint8_t) BIT(*bit_no);

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

unsigned int check_data() {
  uint8_t status_reg_content;

  if (util_sys_inb(STATUS_REG, &status_reg_content) != 0) return 1;

  if (status_reg_content & (BIT(6) | BIT(7))) return 1;

  if (((status_reg_content & OBF) == 0 && (status_reg_content & BIT(5)) == 0)) return 2;

  return 0;
}

void (mouse_ih)() {
  // No need to check the OBF or AUX bits
  util_sys_inb(OUT_BUF, &mouse_code);
} 

int mouse_polling(){
  uint8_t status_reg_content = 0, obf_content = 0;
  

  uint8_t n = 0;
  while (true) {

    if (util_sys_inb(STATUS_REG, &status_reg_content) != 0) return 1;

    if (status_reg_content & (BIT(7) | BIT(6))) return 1;

    if (status_reg_content & OBF && status_reg_content & BIT(5)){

      if (util_sys_inb(OUT_BUF, &obf_content) != 0) return 1;
      bytes_read[n] = obf_content;
      printf("%x\n", obf_content);
      if (n == 2) break;
      n++;

    }
    tickdelay(micros_to_ticks(DELAY_US));
    

  }
  return 0;
}
