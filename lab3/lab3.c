#include <lcom/lcf.h>
#include <minix/sysutil.h>

#include <stdbool.h>
#include <stdint.h>
#include "Macros.h"
#include "keyboard.h"
#include "utils.h"

extern uint8_t kbd_code;  
extern unsigned int sys_inb_counter;
unsigned int timer_counter = 0;

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

  if (util_sys_inb(STATUS_REG, &status_data) != 0) error = true;  // Reads the Status Reg

  if (status_data & (BIT(7) | BIT(6) | BIT(5))) error = true; // Checks parity error, Timeout error and Mouse data

  if (status_data & BIT(0)){ // Makes sure that Output buffer is full
    if (util_sys_inb(OUT_BUF, &kbd_code) != 0) error = true;
  }

  if (kbd_code == LARGEST_NUM)
    kbd_code = 0;
  if (error)
    kbd_code = 0;
}


/*int (polling)() {

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
}*/


int(kbd_test_scan)() {
  // Interrupt handling variables
  int ipc_status;   // gets ipc_status
  int r;   // return value of driver receive
  message msg;
  uint8_t irq_set = BIT(0); // Keyboard's IRQ

  // Used for scan code logic
  uint8_t msbit;   //  holds the most significant bit of scan code
  bool two_bytes = false, make = false;  //  two_bytes - whether the scan code is made of two bytes or one
                                         //  make - whether the scan code is a make or break code
  uint8_t bytes[2];    // holds the scan code

  if (kbd_subscribe_int(& irq_set) != 0) return 1;  // Subscribes keyboard interruptions

  while (kbd_code != ESC_break)    //   Program exits when break code of escape key is read
  {
    kbd_code = 0;    //  Resets kbd_code

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
            kbc_ih();    // Handles the interrupt
            if (kbd_code == 0)    // Checks for erros in kbd_code
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
      msbit = util_get_MSbit(kbd_code);    // To check whether it is a make or break code

      if (msbit != 1)
        make = true;

      if (two_bytes){
        bytes[0] = BYTE2_CODE;
        bytes[1] = kbd_code;
        kbd_print_scancode(make,2,bytes);
      } else {
        bytes[0] = kbd_code;
        kbd_print_scancode(make,1,bytes);
      }
    }

    two_bytes = false; make = false;   // reseting state variables
  }  // end of interrupt loop


  if (kbd_unsubscribe_int() != 0) return 1;   // Unsubscribing keyboard interruptions
  kbd_print_no_sysinb(sys_inb_counter);   // Prints the number of calls to util_sys_inb

  return 0;

}

int(kbd_test_poll)() {
  // Used for scan code logic 
  uint8_t msbit;   //  holds the most significant bit of scan code
  bool two_bytes = false, make = false;  //  two_bytes - whether the scan code is made of two bytes or one
                                         //  make - whether the scan code is a make or break code
  uint8_t bytes[2];    // holds the scan code
  uint8_t status_data; // Will hold the status reg data

  while (kbd_code != ESC_break)
  {
    kbd_code = 0;

    //if (polling() != 0) return 1;

    /*
    This is the supposed polling function.
    Initially, this was a separated function, but we were segmentation errors when it was called (and didn't find a better solution).
    Therefore, we moved the body of the polling function to this location, where it works perfectly.
    */
    // START of polling "function"
    while (true) {
      if (util_sys_inb(STATUS_REG, &status_data) != 0){
        return 1;
      }

      if (status_data & (BIT(7) | BIT(6) |BIT(5))) { // Checks parity error, Timeout error and Mouse data
        return 1;
      }

      if (status_data & OBF) { // Checks if Output buffer is full
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
    // END of polling "function"

    if (kbd_code == BYTE2_CODE){
      two_bytes = true;
      continue;
    }
    if (kbd_code != 0){
      msbit = util_get_MSbit(kbd_code);
      if (msbit != 1)
        make = true;

      if (two_bytes){
        bytes[0] = BYTE2_CODE;
        bytes[1] = kbd_code;
        kbd_print_scancode(make,2,bytes);
      } else {
        bytes[0] = kbd_code;
        kbd_print_scancode(make,1, bytes);
      }
    }

    two_bytes = false; make = false;
  }

  if (enable_interrupts() != 0) return 1;
  kbd_print_no_sysinb(sys_inb_counter);
  return 0;
}

void (timer_int_handler)() {
  timer_counter++;
}

int(kbd_test_timed_scan)(uint8_t n) {
  // Interrupt handling variables
  int ipc_status;   // gets ipc_status
  int r;   // return value of driver receive
  message msg;
  uint8_t irq_kbd = BIT(0); // Keyboard's IRQ
  uint8_t irq_timer0 = BIT(1); // Timer0's IRQ

  // Used for scan code logic
  uint8_t msbit;
  bool two_bytes = false, make = false;
  uint8_t bytes[2];


  if (kbd_subscribe_int(& irq_kbd) != 0) return 1;  // Subscribes KBD interruptions
  if (timer_subscribe_int(& irq_timer0) != 0) return 1;  // Subscribes Timer0 interruptions


  while (kbd_code != ESC_break && n > timer_counter/60)  //  Exits program when break code of escape key is read or keyboard input is idle for more than "n" seconds
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
          if (msg.m_notify.interrupts & irq_kbd)   // KDB interrupt received
          {
            kbc_ih();
            if (kbd_code == 0)
              return 1;
            timer_counter = 0;

          }
          if (msg.m_notify.interrupts & irq_timer0)   // Timer0 interrupt received
          {
            timer_int_handler();
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
      msbit = util_get_MSbit(kbd_code);
      if (msbit != 1)
        make = true;


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

  if (kbd_unsubscribe_int() != 0) return 1;  // unsubscribes KBD interrupts
  if (timer_unsubscribe_int() != 0) return 1;  // unsubscribes Timer0 interrupts

  kbd_print_no_sysinb(sys_inb_counter);

  return 0;
}
