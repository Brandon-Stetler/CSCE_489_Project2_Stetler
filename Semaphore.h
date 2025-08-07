#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <pthread.h> // Threads library for mutexes and condition variables

// Semaphore class provides counting semaphore implementation
class Semaphore 
{
public:
	// Counstructor that initializes semaphore count and associated pthread objects
	Semaphore(int count = 0); 

	// Destructor that cleans up pthread objects
	~Semaphore();

	// wait() decrements the semaphore count, and blocks if count == 0
	void wait();

	// signal() increments the semaphore count, and wakes a blocked thread if one is available
	void signal();

private:
	pthread_mutex_t mutex; // ensures atomic access to count
	pthread_cond_t cond; // condition variable for thread blocking and waiting
	int count; // current number of available resources

};

#endif // end of SEMAPHORE.h
