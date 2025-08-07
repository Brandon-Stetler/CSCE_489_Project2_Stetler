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
    pthread_mutex_lock(&mutex); // lock mutex
    while (count == 0) { // when no resources are available...
        pthread_cond_wait(&cond, &mutex); //block this thread, releasing mutex until signal
    }
    --count; // consume one resource
    pthread_mutex_unlock(&mutex); // release mutex

}


/*************************************************************************************
 * signal - implement a standard signal Semaphore method here
 *
 *************************************************************************************/

void Semaphore::signal() {
    pthread_mutex_lock(&mutex); // lock mutex to modify count safely
    ++count; // put one resource back
    pthread_cond_signal(&cond); // wake up one blocked thread, if present
    pthread_mutex_unlock(&mutex); // release mutex

}


