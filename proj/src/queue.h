#include <lcom/lcf.h>
#include <minix/driver.h>
#include <stdint.h>
#include <stdio.h>

/** @defgroup Queue Queue
 * @{
 * Contains functions for charqueue
 */

/**
 * @brief struct for charqueue node
 */
typedef struct {
    uint8_t data;  /**< @brief holds the data of the element */
    void * next;   /**< @brief address of the next element in the charqueue */
} charqueue_node;

/**
 * @brief struct for charqueue
 */
typedef struct {    
    charqueue_node * first;   /**< @brief pointer to the first element in the charqueue */
    charqueue_node * last;    /**< @brief pointer to the last element in the charqueue */
    unsigned size;            /**< @brief size of the charqueue */
} charqueue;


/**
 * @brief constructor method for charqueue
 * @return pointer to the charqueue created
 */
charqueue * create_charqueue(); 

/**
 * @brief prints a charqueue
 * @param q the charqueue to print
 */
void charqueue_print(charqueue *q);

/**
 * @brief PUSH method for the charqueue
 * @param q the charqueue that will receive an element
 * @param value the value to be added
 */
void charqueue_push(charqueue *q, uint8_t value);

/**
 * @brief POP method for the charqueue
 * @param q the charqueue
 * @return the value in the frontof the queue
 */
uint8_t charqueue_pop(charqueue *q);

/**
 * @brief GET method for the element in front of the queue
 * @param q the charqueue
 * @return the value
 */
uint8_t charqueue_front(charqueue *q);

/**
 * @brief makes a charqueue empty
 * @param q the charqueue
 */
void charqueue_make_empty(charqueue *q);

/**
 * @brief checks if the charqueue is empty
 * @param q the charqueue
 * @return whether it is empty or not
 */
bool charqueue_empty(charqueue *q);

/**
 * @brief gets the size of the charqueue
 * @param q the charqueue
 * @return the size of the charqueue
 */
unsigned charqueue_size(charqueue *q);

/**
 * @brief removes the last element from the charqueue
 * @param q charqueue
 */
void charqueue_remove_last(charqueue *q);

/**
 * @brief converts a charqueue to a string
 * @return the string result
 */
char * charqueue_to_string(charqueue *q);

/** @}*/
