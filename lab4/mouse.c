#include <lcom/lcf.h>

#include <stdint.h>

#include "Macros.h"

int hook_id_mouse = 0;
uint8_t mouse_code;
uint8_t bytes_read[3];
int hook_id_timer = TIMER_SEL0;
unsigned int timer_counter = 0;

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


int (mouse_subscribe_int)(uint8_t *bit_no){
  hook_id_mouse = (int) *bit_no; // saves bit_no value
  
  // subscribe a notification on interrupts
  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &hook_id_mouse) != 0)
    return 1;
  
  *bit_no = (uint8_t) BIT(*bit_no);

  return 0;
}

int (mouse_unsubscribe_int)() {
  // removes notification subscriptions on interrupts
  if (sys_irqrmpolicy(&hook_id_mouse) != 0)
    return 1;

  return 0;
}

int write_command_byte(uint8_t cmd){
  uint8_t status_reg_content = 0;
  
  if (util_sys_inb(STATUS_REG, &status_reg_content) != 0) return 1;  // Reads status register

  if ((status_reg_content & BIT(1)) == 0) {    //  Makes sure that input buffer isn't full
  
    if (sys_outb(STATUS_REG, WRITE_CMD_BYTE) != 0) return 1;    // sends information that command byte will be written    
  
  } else { return 1; }

  if (sys_outb(OUT_BUF, cmd) != 0) return 1;    // sends command byte through outbuffer
  return 0;
}


int send_command_to_mouse(uint8_t cmd){
  uint8_t status_reg_content = 0, response = 0;
  do {

    if (util_sys_inb(STATUS_REG, &status_reg_content) != 0) return 1;
    if ((status_reg_content & IBF) != 0) continue;

    if (sys_outb(STATUS_REG, MS_WRITE_BYTE_CMD) != 0) return 1;

    if (util_sys_inb(STATUS_REG, &status_reg_content) != 0) return 1;
    if ((status_reg_content & IBF) != 0) continue;

    if (sys_outb(OUT_BUF, cmd) != 0) return 1;


    if (util_sys_inb(STATUS_REG, &status_reg_content) != 0) return 1;
    if (status_reg_content & OBF && status_reg_content & AUX) {

      if (util_sys_inb(OUT_BUF, &response) != 0) return 1;
      if (response == ERROR) return 1;

    }

  } while (response != ACK);

  return 0;
}


void parse_packet(struct packet *mouse_data) {
  mouse_data->rb = (mouse_data->bytes[0] & RB_BIT);
  mouse_data->lb = (mouse_data->bytes[0] & LB_BIT);
  mouse_data->mb = (mouse_data->bytes[0] & MB_BIT);
  mouse_data->x_ov = (mouse_data->bytes[0] & X_OVF);
  mouse_data->y_ov = (mouse_data->bytes[0] & Y_OVF);

  if (mouse_data->bytes[0] & MSB_X_DELTA) {
    mouse_data->delta_x = mouse_data->bytes[1] | (LARGEST_NUM << 8);
  }
  else {
    mouse_data->delta_x = mouse_data->bytes[1];
  }
  if (mouse_data->bytes[0] & MSB_Y_DELTA) {
    mouse_data->delta_y = ((LARGEST_NUM << 8) | mouse_data->bytes[2]);
  }
  else {
    mouse_data->delta_y = mouse_data->bytes[2];
  }
}


void (mouse_ih)() {
  // No need to check the OBF or AUX bits
  util_sys_inb(OUT_BUF, &mouse_code);
} 

void (timer_int_handler)() {
  timer_counter++;
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
      //printf("%x\n", obf_content);
      if (n == 2) break;
      n++;

    }
    tickdelay(micros_to_ticks(DELAY_US));
    

  }
  return 0;
}
