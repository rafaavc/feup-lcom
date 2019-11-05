// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "utils.h"
#include "mouse.h"
#include "Macros.h"

unsigned int sys_inb_counter = 0;
extern uint8_t mouse_code;
extern uint8_t bytes_read[];
extern int hook_id_mouse;

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
  uint8_t irq_set = BIT(1); // Mouse's IRQ
  unsigned int counter = 0, byte_counter = 0;
  //uint8_t mouse_code;
  uint8_t prev = 0;
  struct packet mouse_data;
  //uint8_t status_reg_content;
  //bool error;

  //printf("1\n");
  //send_statusreg_command(ENABLE_MOUSE, 0);
  mouse_enable_data_reporting();
  /*if (send_statusreg_commandbyte(BIT(1), 0)!=0) printf("error\n");
  if (send_statusreg_command(MS_WRITE_BYTE_CMD, ENABLE_STREAM_MODE)!= 0) printf("error\n");
  if (send_statusreg_command(MS_WRITE_BYTE_CMD, ENABLE_DATA_REPORTING)!= 0) printf("error\n");*/

  //printf("2\n");
  if ((mouse_subscribe_int)(& irq_set) != 0) return 1;  // Subscribes mouse interruptions
  //printf("3\n");

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
            mouse_ih();
            printf("MOUSE CODE: %x\n", mouse_code);

            //if (!err) {
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
              prev = mouse_code;
            //}
          }
          break;
        default:
          break;
      }
    }
  printf("BYTE COUNTER: %d\n", byte_counter);
  }  // end of interrupt loop

  if ((mouse_unsubscribe_int)() != 0) return 1;   // Unsubscribing mouse interruptions

  /*send_statusreg_command(MS_WRITE_BYTE_CMD, DISABLE_DATA_REPORTING);
  send_statusreg_command(MS_WRITE_BYTE_CMD, SET_DEFAULTS);
  send_statusreg_command(DISABLE_MOUSE, 0);*/
  return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
  unsigned int nada = 0;
  struct packet mouse_data;


  while(nada < cnt){
    nada++;
    if (send_statusreg_command(READ_MOUSE_DATA,0) != 0) return 1;

    if (mouse_polling() != 0) return 1;

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
