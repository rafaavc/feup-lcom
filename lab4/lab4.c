// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "utils.h"
#include "mouse.h"
#include "Macros.h"

unsigned int sys_inb_counter = 0;
extern uint8_t mouse_code;
extern bool error;
extern uint8_t bytes_read[3];

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
  //uint8_t status_reg_content;
  printf("1\n");
  mouse_enable_data_reporting();
  printf("2\n");
  if (mouse_subscribe_int(& irq_set) != 0) return 1;  // Subscribes mouse interruptions
  printf("3\n");

  while (counter < cnt)    //   Program exits when cnt number of packets are read
  {
    printf("4\n");
    if ( (r = driver_receive(ANY, &msg, &ipc_status) != 0))
    {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    printf("5\n");
    if (is_ipc_notify(ipc_status))
    {
      switch (_ENDPOINT_P(msg.m_source))
      {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set)
          {
            printf("6\n");
            (mouse_ih)();
            // ***************
            /*
            if (util_sys_inb(STATUS_REG, &status_reg_content) != 0) error = true;

            if (status_reg_content & (BIT(6) | BIT(7))) error = true;

            if ((status_reg_content & OBF) && !error && (status_reg_content & BIT(5))) {
              
              if (sys_outb(OUT_BUF, mouse_code) != 0) error = true;

            } else { error = true; }
            */
            // ***************


            printf("7\n");
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
  
  printf("8\n");
  if (mouse_unsubscribe_int() != 0) return 1;   // Unsubscribing mouse interruptions
  
  return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
  unsigned int nada = 0;
  struct packet mouse_data;


  while(nada < cnt){
    nada++;
    if (send_statusreg_command(READ_MOUSE_DATA,0) != 0) return 1;

    if (mouse_polling(period) != 0) return 1;
    if (true){       //If BIT(3) != 1 certainly not first byte
      mouse_data.bytes[0] = bytes_read[0];
      //if (mouse_polling() != 0) return 1;
      mouse_data.bytes[1] = bytes_read[1];
      //if (mouse_polling() != 0) return 1;
      mouse_data.bytes[2] = bytes_read[2];
      mouse_data.rb = (mouse_data.bytes[0] & RB_BIT);
      mouse_data.lb = (mouse_data.bytes[0] & LB_BIT);
      mouse_data.mb = (mouse_data.bytes[0] & MB_BIT);
      mouse_data.x_ov = (mouse_data.bytes[0] & X_OVF);
      mouse_data.y_ov = (mouse_data.bytes[0] & Y_OVF);
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
    }
    tickdelay(micros_to_ticks(period*1000));
    printf("%d\n",nada);
  }

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
