#include "opt-synchprobs.h"
#include <types.h>  /* required by lib.h */
#include <lib.h>    /* for kprintf */
#include <synch.h>  /* for P(), V(), sem_* */
#include <thread.h> /* for thread_fork() */
#include <test.h>

#include "dining_driver.h"

/*
 * Declare any data structures you might need to synchronise 
 * your forks here.
 */
#define LEFT (phil_num + NUM_PHILOSOPHERS-1)%NUM_PHILOSOPHERS
#define RIGHT (phil_num+1)%NUM_PHILOSOPHERS

struct semaphore* mutex;
struct semaphore* phils[NUM_PHILOSOPHERS];
int state[NUM_PHILOSOPHERS];


static void test_phil(unsigned long phil_num){
    if(state[phil_num] == 1
    && state[LEFT] != 2
    && state[RIGHT] != 2){
        state[phil_num] = 2;
        V(phils[phil_num]);
    }
}

/*
 * Take forks ensures mutually exclusive access to two forks
 * associated with the philosopher.
 * 
 * The left fork number = phil_num
 * The right fork number = (phil_num + 1) % NUM_PHILOSPHERS
 */

void take_forks(unsigned long phil_num)
{
    P(mutex);
    state[phil_num] = 1;
    test_phil(phil_num);
    V(mutex);
    P(phils[phil_num]);
}


/*
 * Put forks releases the mutually exclusive access to the
 * philosophers forks.
 */

void put_forks(unsigned long phil_num)
{
    P(mutex);
    state[phil_num] = 0;
    test_phil(LEFT);
    test_phil(RIGHT);
    V(mutex);
}


/* 
 * Create gets called before the philosopher threads get started.
 * Insert any initialisation code you require here.
 */

void create_forks()
{
    mutex = sem_create("mutex for phil", 1);
    for(int i = 0; i < NUM_PHILOSOPHERS; i++) {
        state[i] = 0;
        phils[i] = sem_create("a phil sem", 1);
        if(phils[i] == NULL){
            panic("A fork could not be created");
        }
    }
    
}


/*
 * Destroy gets called when the system is shutting down.
 * You should clean up whatever you allocated in create_forks()
 */

void destroy_forks()
{
    sem_destroy(mutex);
    for(int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_destroy(phils[i]);
    }
}
