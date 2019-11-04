// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "utils.h"
#include "mouse.h"
#include "Macros.h"

unsigned int sys_inb_counter = 0;
extern uint8_t mouse_code;

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
    int ipc_status;   // gets ipc_status
  int r;   // return value of driver receive
  message msg;
  uint8_t irq_set = BIT(0); // Mouse's IRQ
  unsigned int counter = 0, byte_counter = 0;
  struct packet mouse_data;

  mouse_enable_data_reporting();
  if (mouse_subscribe_int(& irq_set) != 0) return 1;  // Subscribes mouse interruptions


  while (counter < cnt)    //   Program exits when cnt number of packets are read
  {
    if ( (r = driver_receive(ANY, &msg, &ipc_status) != 0))
    {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status))
    {
      switch (_ENDPOINT_P(msg.m_source))
      {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set)
          {
            (mouse_ih)();
            if ((mouse_code & BIT(3)) && byte_counter == 0){       //If BIT(3) != 1 certainly not first byte
              mouse_data.bytes[0] = mouse_code;
              byte_counter++;
            }
            else if (byte_counter == 1){
              mouse_data.bytes[1] = mouse_code;
              byte_counter++;
            }
            else if (byte_counter == 2){
              mouse_data.bytes[2] = mouse_code;
              mouse_data.rb = (mouse_data.bytes[0] & RB_BIT);
              mouse_data.lb = (mouse_data.bytes[0] & LB_BIT);
              mouse_data.mb = (mouse_data.bytes[0] & MB_BIT);
              mouse_data.x_ov = (mouse_data.bytes[0] & X_OVF);
              mouse_data.y_ov = (mouse_data.bytes[0] & Y_OVF);
              /*mouse_data.delta_x = join_bytes(mouse_data.bytes[0] & MSB_X_DELTA, mouse_data.bytes[1]);
              mouse_data.delta_y = join_bytes(mouse_data.bytes[0] & MSB_Y_DELTA, mouse_data.bytes[2]);*/

              if (mouse_data.bytes[0] & MSB_X_DELTA) {
                mouse_data.delta_x = mouse_data.bytes[1] | (LARGEST_NUM << 8);
              }
              else {
                mouse_data.delta_x = mouse_data.bytes[1];
              }
              if (mouse_data.bytes[0] & MSB_Y_DELTA) {
                mouse_data.delta_y = ((LARGEST_NUM << 8) | mouse_data.bytes[2]);
              }
              else {
                mouse_data.delta_y = mouse_data.bytes[2];
              }
              mouse_print_packet(&mouse_data);
              counter++;
              byte_counter = 0;
            }
          }
          break;
        default:
          break;
      }
    }
  }  // end of interrupt loop

  if (mouse_unsubscribe_int() != 0) return 1;   // Unsubscribing mouse interruptions

  return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
  unsigned int counter = 0, byte_counter = 0;
  struct packet mouse_data;

  while (counter < cnt)    //   Program exits when cnt number of packets are read
  {
    tickdelay(micros_to_ticks(period));
    if (mouse_polling() != 0) return 1;
    printf("Did polling\n");
    if ((mouse_code & BIT(3)) && byte_counter == 0){       //If BIT(3) != 1 certainly not first byte
      mouse_data.bytes[0] = mouse_code;
      byte_counter++;
      if (mouse_polling(period) != 0) return 1;
    
      mouse_data.bytes[1] = mouse_code;
      byte_counter++;
      if (mouse_polling(period) != 0) return 1;
    
      mouse_data.bytes[2] = mouse_code;
      mouse_data.rb = (mouse_data.bytes[0] & RB_BIT);
      mouse_data.lb = (mouse_data.bytes[0] & LB_BIT);
      mouse_data.mb = (mouse_data.bytes[0] & MB_BIT);
      mouse_data.x_ov = (mouse_data.bytes[0] & X_OVF);
      mouse_data.y_ov = (mouse_data.bytes[0] & Y_OVF);
      /*mouse_data.delta_x = join_bytes(mouse_data.bytes[0] & MSB_X_DELTA, mouse_data.bytes[1]);
      mouse_data.delta_y = join_bytes(mouse_data.bytes[0] & MSB_Y_DELTA, mouse_data.bytes[2]);*/

      if (mouse_data.bytes[0] & MSB_X_DELTA) {
        mouse_data.delta_x = mouse_data.bytes[1] | (LARGEST_NUM << 8);
      }
      else {
        mouse_data.delta_x = mouse_data.bytes[1];
      }
      if (mouse_data.bytes[0] & MSB_Y_DELTA) {
        mouse_data.delta_y = ((LARGEST_NUM << 8) | mouse_data.bytes[2]);
      }
      else {
        mouse_data.delta_y = mouse_data.bytes[2];
      }
      mouse_print_packet(&mouse_data);
      counter++;
      byte_counter = 0;

    }
  }  // end of interrupt loop


  // reset mouse to stream mode and disable mouse data reporting
  uint8_t dflt_kbc_cmd_byte = minix_get_dflt_kbc_cmd_byte();
  sys_outb(STATUS_REG, dflt_kbc_cmd_byte);

  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    /* To be completed */
    printf("%s(%u): under construction\n", __func__, idle_time);
    return 1;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    /* To be completed */
    printf("%s: under construction\n", __func__);
    return 1;
}
