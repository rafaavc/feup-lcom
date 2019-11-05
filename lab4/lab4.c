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
extern unsigned int timer_counter;

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
  struct packet mouse_data;

  send_command_to_mouse(ENABLE_DATA_REPORTING);

  if ((mouse_subscribe_int)(& irq_set) != 0) return 1;  // Subscribes mouse interruptions

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
              parse_packet(&mouse_data);
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

  if ((mouse_unsubscribe_int)() != 0) return 1;   // Unsubscribing mouse interruptions

  send_command_to_mouse(DISABLE_DATA_REPORTING);

  return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
  unsigned int counter = 0;
  struct packet mouse_data;

  while(counter < cnt){
    if (send_command_to_mouse(READ_MOUSE_DATA) != 0) return 1;

    if (mouse_polling() != 0) return 1;

    if (true){       //If BIT(3) != 1 certainly not first byte
      mouse_data.bytes[0] = bytes_read[0];
      mouse_data.bytes[1] = bytes_read[1];
      mouse_data.bytes[2] = bytes_read[2];

      parse_packet(&mouse_data);
      mouse_print_packet(&mouse_data);
    }
    tickdelay(micros_to_ticks(period*1000));
    counter++;
  }

  send_command_to_mouse(SET_STREAM_MODE);
  send_command_to_mouse(DISABLE_DATA_REPORTING);
  write_command_byte(minix_get_dflt_kbc_cmd_byte());

  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
  int ipc_status;   // gets ipc_status
  int r;   // return value of driver receive
  message msg;
  uint8_t irq_set_mouse = BIT(1), irq_set_timer = BIT(2); // Mouse's and timer's IRQ

  unsigned int counter = 0, byte_counter = 0;
  struct packet mouse_data;

  send_command_to_mouse(ENABLE_DATA_REPORTING);

  if ((mouse_subscribe_int)(& irq_set_mouse) != 0) return 1;  // Subscribes mouse interruptions
  if ((timer_subscribe_int)(& irq_set_timer) != 0) return 1;  // Subscribes timer interruptions

  while (timer_counter/sys_hz() < idle_time)    //   Program exits when cnt number of packets are read
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
          if (msg.m_notify.interrupts & irq_set_mouse)
          {
            mouse_ih();

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
              parse_packet(&mouse_data);
              mouse_print_packet(&mouse_data);
              counter++;
              byte_counter = 0;
            }
            timer_counter = 0;
          }
          if (msg.m_notify.interrupts & irq_set_timer) {
            timer_int_handler();
          }
          break;
        default:
          break;
      }
    }
  }  // end of interrupt loop

  if ((mouse_unsubscribe_int)() != 0) return 1;   // Unsubscribing mouse interruptions
  if ((timer_unsubscribe_int)() != 0) return 1;   // Unsubscribing timer interruptions

  send_command_to_mouse(DISABLE_DATA_REPORTING);

  return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  int ipc_status;   // gets ipc_status
  int r;   // return value of driver receive
  message msg;
  uint8_t irq_set = BIT(1); // Mouse's IRQ
  byte_counter = 0;
  struct packet mouse_data;

  send_command_to_mouse(ENABLE_DATA_REPORTING);

  if ((mouse_subscribe_int)(& irq_set) != 0) return 1;  // Subscribes mouse interruptions

  while (true)    //   Program exits when cnt number of packets are read
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
              parse_packet(&mouse_data);
              mouse_print_packet(&mouse_data);
              byte_counter = 0;
            }
          }
          break;
        default:
          break;
      }
    }
  }  // end of interrupt loop

  if ((mouse_unsubscribe_int)() != 0) return 1;   // Unsubscribing mouse interruptions

  send_command_to_mouse(DISABLE_DATA_REPORTING);

  return 0;
}
