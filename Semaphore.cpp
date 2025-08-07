#include <pthread.h>
#include "Semaphore.h"


/*************************************************************************************
 * Semaphore (constructor) - this should take count and place it into a local variable.
 *						Here you can do any other initialization you may need.
 *
 *    Params:  count - initialization count for the semaphore
 *
 *************************************************************************************/

Semaphore::Semaphore(int count) : count (count) {
    pthread_mutex_init(&mutex, nullptr); // initialize mutex with default attributes
    pthread_cond_init(&cond, nullptr); // initialize condition variable with default attributes
}


/*************************************************************************************
 * ~Semaphore (destructor) - called when the class is destroyed. Clean up any dynamic
 *						memory.
 *
 *************************************************************************************/

Semaphore::~Semaphore() {
    pthread_mutex_destroy(&mutex); // destroy the mutex
    pthread_cond_destroy(&cond); // destroy the condition variable
}


/*************************************************************************************
 * wait - implement a standard wait Semaphore method here
 *
 *************************************************************************************/

void Semaphore::wait() {

}


/*************************************************************************************
 * signal - implement a standard signal Semaphore method here
 *
 *************************************************************************************/

void Semaphore::signal() {

}


