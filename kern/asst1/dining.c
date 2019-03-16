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

//I felt an enum would be better than 3 #define for the diffrent states
enum phil_state {thinking, hungry, eating}; 

//Finds the unsinged long value for the philosopher to the left and right
//Macros are error-prone because they rely on textual substitution and do not perform type-checking
static int left(unsigned long i){
    return (int)(i + NUM_PHILOSOPHERS-1)%NUM_PHILOSOPHERS;
}
static int right(unsigned long i){
    return (int)(i + 1)%NUM_PHILOSOPHERS;
}
//

//Mutex for the dining philosophers
struct semaphore* mutex;
//A semaphore for all the philosofers
struct semaphore* phils[NUM_PHILOSOPHERS];

//Keeps track on the diffrent states of the philosophers
int state[NUM_PHILOSOPHERS];

//check if the a said philosopher is hungry and
// philosophers next to said philosopher is not busy eating
//If this is true then our said philosopher can start eating
//Also up the said philospher
static void test_phil(unsigned long phil_num){
    if(state[phil_num] == hungry
    && state[left(phil_num)] != eating
    && state[right(phil_num)] != eating){
        state[phil_num] = eating;
        //Up the philosopher semaphore
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
    //Down mutex
    P(mutex);
    state[phil_num] = hungry;
    test_phil(phil_num);
    //Up mutex
    V(mutex);
    //Down the philosopher semaphore
    P(phils[phil_num]);
}


/*
 * Put forks releases the mutually exclusive access to the
 * philosophers forks.
 */

void put_forks(unsigned long phil_num)
{
    //Down mutex
    P(mutex);
    state[phil_num] = thinking;
    test_phil(left(phil_num));
    test_phil(right(phil_num));
    //Up mutex
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
        state[i] = thinking;
        phils[i] = sem_create("a phil nr sem", 1);
        if(phils[i] == NULL){
            panic("fork nr %d could not be created", i);
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
