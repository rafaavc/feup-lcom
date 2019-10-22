#include <lcom/lcf.h>
#include <minix/sysutil.h>

#include <stdbool.h>
#include <stdint.h>
#include "Macros.h"
#include "keyboard.h"
#include "utils.h"

extern uint8_t kbd_code;  
extern unsigned int sys_inb_counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

void (kbc_ih)(){
  uint8_t status_data;
  bool error = false;

  if (util_sys_inb(STATUS_REG, &status_data) != 0){  // Reads the Status Reg
    error = true;
  }

  if (status_data & (BIT(7) | BIT(6) |BIT(5))){ // Checks parity error, Timeout error and Mouse data
    error = true;
  }

  if (status_data & BIT(0)){ // Checks if Output buffer is full
    if (util_sys_inb(OUT_BUF, &kbd_code) != 0)
      error = true;
  }

  if (kbd_code == LARGEST_NUM)
    kbd_code = 0;
  if (error)
    kbd_code = 0;

  //tickdelay(micros_to_ticks(DELAY_US));
}


int (polling)() {

  printf("STARTED POLLING\n");
  uint8_t status_data;
  while (true) {
    if (util_sys_inb(STATUS_REG, &status_data) != 0){
      return 1;
    }

    if (status_data & (BIT(7) | BIT(6) |BIT(5))){ // Checks parity error, Timeout error and Mouse data
      return 1;
    }

    if (status_data & OBF){ // Checks if Output buffer is full
      printf("OUTPUT BUFFER IS FULL \n");
      if (util_sys_inb(OUT_BUF, &kbd_code) != 0) return 1;
      printf("GOT KBD CODE\n");
      if (kbd_code == LARGEST_NUM) return 1;

      //break;
      return 0;
    } else {
      printf("OUTPUT BUFFER WASN'T FULL\n");
      tickdelay(micros_to_ticks(DELAY_US));

    }
  }
  return 0;
}


int(kbd_test_scan)() {
  int ipc_status;
  int r;
  message msg;
  uint8_t irq_set = BIT(0); // IRQ1
  uint8_t msbit;
  bool two_bytes = false, make = false;
  uint8_t bytes[2];

  if (kbd_subscribe_int(& irq_set) != 0)
    return 1;

  while (kbd_code != ESC_break)
  {
    kbd_code = 0;
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
            kbc_ih();
            if (kbd_code == 0)
              return 1;

          }
          break;

        default:
          break;
      }
    }

    if (kbd_code == BYTE2_CODE){
      two_bytes = true;
      continue;
    }
    
    if (kbd_code != 0){

      util_get_MSbit(kbd_code, &msbit);
      if (msbit != 1)
        make = true;
      else make = false;


      if (two_bytes){
        bytes[0] = BYTE2_CODE;
        bytes[1] = kbd_code;
        kbd_print_scancode(make,2,bytes);
      }
      else{
        bytes[0] = kbd_code;
        kbd_print_scancode(make,1,bytes);
      }
    }

    two_bytes = false; make = false;

  }

  if (kbd_unsubscribe_int() != 0)
    return 1;

  
  kbd_print_no_sysinb(sys_inb_counter);

  return 0;

}

int(kbd_test_poll)() {
  uint8_t msbit = 0;
  bool two_bytes = false, make = false;
  uint8_t bytes[2];
  uint8_t status_data; // TEST
  //bytes[0] = 0x56; // TEST
  //printf("STARTED\n");
  //kbd_print_scancode(true, 1, bytes);
  //printf("PASSED\n");
  while (kbd_code != ESC_break)
  {
    //printf("LOOP BEGIN\n");
    kbd_code = 0;
    //kbd_print_scancode(true, 1, bytes); // WORKED
    /*if (polling() != 0)
      return 1;*/

    // START

    
    while (true) {
      if (util_sys_inb(STATUS_REG, &status_data) != 0){
        return 1;
      }

      if (status_data & (BIT(7) | BIT(6) |BIT(5))){ // Checks parity error, Timeout error and Mouse data
        return 1;
      }

      if (status_data & OBF){ // Checks if Output buffer is full
        //printf("OUTPUT BUFFER IS FULL \n");
        if (util_sys_inb(OUT_BUF, &kbd_code) != 0) return 1;
        //printf("GOT KBD CODE\n");
        if (kbd_code == LARGEST_NUM) return 1;

        break;
      } else {
        //printf("OUTPUT BUFFER WASN'T FULL\n");
        tickdelay(micros_to_ticks(DELAY_US));

      }
    }

    // END


    //printf("POLLING CALLED\n");
    //kbd_print_scancode(true, 1, bytes); // ERROR
    if (kbd_code == BYTE2_CODE){
      two_bytes = true;
      continue;
    }
    //printf("BEFORE KBD_CODE != 0\n");
    if (kbd_code != 0){
      //printf("got into function\n");
      msbit = kbd_code >> 7;
      //printf("GOT MSbit\n");
      if (msbit != 1)
        make = true;
      if (two_bytes){
        bytes[0] = BYTE2_CODE;
        bytes[1] = kbd_code;
        //printf("starting to print scancode");
        kbd_print_scancode(make,2,bytes);
      } else {
        bytes[0] = kbd_code;
        //printf("starting to print scancode\n");
        kbd_print_scancode(make,1, bytes);
        //printf("asjdasjdgh");
      }
    }

    two_bytes = false; make = false;
  }

  enable_interrupts();

  kbd_print_no_sysinb(sys_inb_counter);

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {

  

  return 1;
}
