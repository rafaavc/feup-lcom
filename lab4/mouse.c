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
  uint8_t status_reg_content = 0, ack = 0;
  while (true) {
    if (util_sys_inb(STATUS_REG, &status_reg_content) != 0) return 1;  // Reads status register

    if ((status_reg_content & BIT(1)) == 0) {    //  Makes sure that input buffer isn't full
      if (sys_outb(STATUS_REG, cmd) != 0) return 1;            // sends command
    } else {
      continue;
    }

    printf("%x\n", status_reg_content);
    if (args != 0) {
      if (sys_outb(OUT_BUF, args) != 0) return 1;    // sends command arguments, if any
    }

    if ((status_reg_content & OBF) && (status_reg_content & BIT(5))) { // if obf and byte from mouse
      printf("obf full and mouse\n");
      if (util_sys_inb(OUT_BUF, &ack) != OK) // read obf (ack byte)
        return 1;
      if (ack == ERROR)
        return 1;
    }
    if (ack == 0xFE) printf("NACK\n"); 
    
    if (args != 0) {
      if (ack == ACK) return 0;
    }
    else {
      return 0;
    }
      
    
  }
}

int mouse_enable_data_reporting_original() {
  send_statusreg_command(ENABLE_MOUSE, 0);
  send_statusreg_command(MS_WRITE_BYTE_CMD, ENABLE_STREAM_MODE);
  send_statusreg_command(MS_WRITE_BYTE_CMD, ENABLE_DATA_REPORTING);
  return 0;
}

int mouse_disable_data_reporting() {
  //send_statusreg_command(MS_WRITE_BYTE_CMD, ENABLE_STREAM_MODE);
  send_statusreg_command(MS_WRITE_BYTE_CMD, DISABLE_DATA_REPORTING);
  send_statusreg_command(DISABLE_MOUSE, 0);
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

  if (util_sys_inb(STATUS_REG, &status_data) != 0) error = true;  // Reads the Status Reg

  if (status_data & (BIT(7) | BIT(6) | !BIT(5))) error = true; // Checks parity error, Timeout error and Mouse data

  if (status_data & BIT(0)){ // Makes sure that Output buffer is full
    if (util_sys_inb(OUT_BUF, &mouse_code) != 0) error = true;
  }
} 

int mouse_polling(){
  uint8_t status_data;
  while (true) {
    if (util_sys_inb(STATUS_REG, &status_data) != 0){
      return 1;
    }
    printf("%x", status_data);

    if (status_data & (BIT(7) | BIT(6) |BIT(5))){ // Checks parity error, Timeout error and Mouse data
      return 1;
    }

    if (status_data & OBF){ // Checks if Output buffer is full
      if (util_sys_inb(OUT_BUF, &mouse_code) != 0) return 1;
      return 0;
    } else {
      tickdelay(micros_to_ticks(DELAY_US));

    }
  }
  return 0;
}
