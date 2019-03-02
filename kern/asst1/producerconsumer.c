/* This file will contain your solution. Modify it as you wish. */
#include <types.h>
#include <lib.h>
#include <synch.h>
#include "producerconsumer_driver.h"

/* Declare any variables you need here to keep track of and
   synchronise your bounded. A sample declaration of a buffer is shown
   below. It is an array of pointers to items.
   
   You can change this if you choose another implementation. 
   However, you should not have a buffer bigger than BUFFER_SIZE 
*/

data_item_t * item_buffer[BUFFER_SIZE];

int count;

struct cv* full;
struct cv* empty;

struct lock* prodconLock;

/* consumer_receive() is called by a consumer to request more data. It
   should block on a sync primitive if no data is available in your
   buffer. */

data_item_t * consumer_receive(void)
{
        
        lock_acquire(prodconLock);
        while(count == 0){
                cv_wait(empty, prodconLock);
        }
        data_item_t * item = item_buffer[count-1];
        count--;
        cv_signal(full, prodconLock);
        lock_release(prodconLock);
        return item;
}

/* procucer_send() is called by a producer to store data in your
   bounded buffer. */

void producer_send(data_item_t *item)
{
        lock_acquire(prodconLock);
        while(count == BUFFER_SIZE){
                cv_wait(full, prodconLock);
        }
        item_buffer[count] = item;
        count++;
        cv_signal(empty,prodconLock);
        lock_release(prodconLock);
        
}




/* Perform any initialisation (e.g. of global data) you need
   here. Note: You can panic if any allocation fails during setup */

void producerconsumer_startup(void)
{
        count = 0;
        full = cv_create("full CV");
        if(full == NULL){
                panic("full CV failed to create");
        }
        empty = cv_create("empty CV");
        if(empty == NULL){
                panic("empty CV failed to create");
        }
        prodconLock = lock_create("Prod Con Lock");
        if(prodconLock == NULL){
                panic("prodconLock failed to create");
        }

}

/* Perform any clean-up you need here */
void producerconsumer_shutdown(void)
{
        cv_destroy(full);
        cv_destroy(empty);
        lock_destroy(prodconLock);
}

