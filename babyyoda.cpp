/*************************************************************************************
 * babyyoda - used to test your semaphore implementation and can be a starting point for
 *			     your store front implementation
 *
 *************************************************************************************/

#include <stdio.h> // used for printf and fprintf
#include <stdlib.h> // used for exit, atoi, malloc, srand, and rand
#include <time.h> // time for srand seed
#include <pthread.h> // used for pthread_create, mutexes, and condition variables
#include <unistd.h> // used for usleep for sleeping threads
#include "Semaphore.h" // Semaphore class created for this project

// Semaphores that each thread will have access to as they are global in shared memory
Semaphore *emptySlots = NULL;
Semaphore *fullSlots = NULL;

// Mutex used to protect access to buffer indices and contents
pthread_mutex_t buf_mutex;

int *buffer; // dynamic array serving as the buffer
int head = 0; // next index for producer to write to
int tail = 0; // next index for consumer to read from

// Struct to pass ID and buffer size to consumer threads
struct ThreadArgs {
	int id; // consumer id
	int buffer_size; // buffer size
};

/*************************************************************************************
 * producer_routine - this function is called when the producer thread is created.
 *
 *			Params: data - points to int[3] = {max_items, buffer_size, num_consumers}
 *			Returns: always NULL
 *
 *************************************************************************************/

void *producer_routine(void *data) {

	int max_items = *((int *)data); // total items to produce, from argument array
	int buffer_size = *((int *)data +1); // buffer size, from argument array
	time_t rand_seed;
	srand((unsigned int) time(&rand_seed));

	// The current Yoda serial number (incremented)
	int serialnum = 1;
	
	// We know the data pointer is an integer that indicates the number to produce
	// int left_to_produce = *((int *) data);

	// Loop through the amount we're going to produce and place into the buffer
	while (true) {

		// Exit the production loop if we've produced all items
		if (serialnum > max_items) {
			break;
		}

		printf("Producer wants to put Yoda #%d into buffer...\n", serialnum);

		emptySlots->wait(); // wait until the buffer has an empty slot

		// Place item on the next shelf slot by first setting the mutex to protect our buffer vars
		pthread_mutex_lock(&buf_mutex); // lock buffer to place item

		// Capture the values actually placed so the print matches the write
		int placed_serial = serialnum;
		int placed_slot = head;

		buffer[head] = serialnum; // store the Yoda id in the buffer
		head = (head + 1) % buffer_size; // advance head, wrapping by buffer size
		serialnum++; // increment serial number

		pthread_mutex_unlock(&buf_mutex); // unlock buffer
		
		fullSlots->signal(); // signal that there's another Yoda in the buffer

		printf("Producer placed Yoda #%d at slot %d\n", placed_serial, placed_slot);
		
		// random sleep but he makes them fast so 1/20 of a second
		usleep((useconds_t) (rand() % 200000));

		// usleep(rand() % 200000); // sleep 0-200 ms to simulate work

		// printf("   Yoda put on shelf.\n");
		
		// Semaphore signal that there are items available
		//empty->signal();
	}

		// After all real items are produced, send a sentinel (-1) to all consumers to let them know
		int num_consumers = *((int *)data + 2); // number of consumers from argument array
		for (int i = 0; i < num_consumers; ++i) {
			emptySlots->wait(); // wait for an empty slot
			pthread_mutex_lock(&buf_mutex); // lock buffer to place sentinel
			buffer[head] = -1; //place sentinel in buffer to signal termination
			head = (head + 1) % buffer_size; // advance head by one
			pthread_mutex_unlock(&buf_mutex); // unlock buffer
			fullSlots->signal(); // signal sentinel availability
		}	
	
	return NULL; // producer thread exits
}


/*************************************************************************************
 * consumer_routine - this function is called when the consumer thread is created.
 *
 *       Params: data - points to ThreadArgs {id, buffer_size}
 *
 *       Returns: always NULL
 *
 *************************************************************************************/

void *consumer_routine(void *data) {
	ThreadArgs *args = (ThreadArgs *)data; // get the passed-in thread arguments
	int id = args->id; // store consumer ID
	int buffer_size = args->buffer_size; // store buffer size
	// (void) data;

	//bool quitthreads = false;

	while (true) {
		printf("Consumer %d wants to buy a Yoda...\n", id); // 

		// Semaphore to see if there are any items to take
		fullSlots->wait(); // wait until there's at least one item

		// Take an item off the shelf
		pthread_mutex_lock(&buf_mutex); // lock buffer to remove item
		int item = buffer[tail]; // read the item at tail
		int slot = tail; // capture slot BEFORE advancing
		tail = (tail + 1) % buffer_size; // advance tail by one

		pthread_mutex_unlock(&buf_mutex); // unlock buffer

		emptySlots->signal(); // signal an empty slot available

		if (item == -1) break; // exit the consumption loop if a sentinel is received

		printf("Consumer %d bought Yoda #%d from slot %d\n", id, item, slot); // Print who bought what from where

		// Consumers wait up to one second
		usleep((useconds_t) (rand() % 1000000));
	}
	printf("Consumer %d goes home (no more Yodas).\n", id);

	return NULL; // consumer thread exits
}


/*************************************************************************************
 * main - Standard C main function for our storefront. 
 *
 *		Expected params: pctest <buffer_size> <num_consumers> <max_items>
 *				max_items - how many items will be produced before the shopkeeper closes
 *
 *************************************************************************************/

int main(int argc, const char *argv[]) {

	setvbuf(stdout, NULL, _IONBF, 0); // disable buffering for stdout

	// Expect exactly three command-line arguments
	if (argc != 4) { // if incorrect number of parameters
		fprintf(stderr, "Invalid parameters. Format: %s <buffer size> <num_consumers> <max_items>\n", argv[0]); // print error message
		return EXIT_FAILURE; // exit with error code
	}

	// User input on the size of the buffer
	int buffer_size = (int) strtol(argv[1], NULL, 10); // convert first arg to buffer size
	int num_consumers = (int) strtol(argv[2], NULL, 10); // convert second arg to number of consumers
	int max_items = (int) strtol(argv[3], NULL, 10); // convert third arg to max items to produce

	printf("Producing %d today.\n", max_items);
	
	// Initialize our semaphores
	emptySlots = new Semaphore(buffer_size);
	fullSlots = new Semaphore(0); // fullSlots starts empty (no items produced yet)

	pthread_mutex_init(&buf_mutex, NULL); // Initialize our buffer mutex
	buffer = new int[buffer_size]; // allocate buffer array

	pthread_t producer; // handle for producer thread
	pthread_t *consumer = new pthread_t[num_consumers]; // array of consumer thread handles
	ThreadArgs *args = new ThreadArgs[num_consumers]; // array of args for for each consumer

	// Place control data into array: [0] = max_items, [1] = buffer size, [2] = num_consumers
	int control[3] = {max_items, buffer_size, num_consumers};

	// Launch our producer thread, passing control[] as an argument
	pthread_create(&producer, NULL, producer_routine, control);

	// Launch each consumer thread, passing a ThreadArgs struct
	for (int i = 0; i < num_consumers; ++i) {
		args[i].id = i + 1; // get id
		args[i].buffer_size = buffer_size; // get buffer_size
		pthread_create(&consumer[i], NULL, consumer_routine, &args[i]);
	}

	// Wait for our producer thread to finish up
	pthread_join(producer, NULL);

	printf("End of producer thread, the manufacturer has completed his work for the day.\n");

	printf("Waiting for consumers to buy up the rest (if there are any).\n");

	// Give the consumers a second to finish snatching up items
	//while (id < max_items)
	//	sleep(1);

	// Now make sure they all exited
//	for (unsigned int i=0; i<NUM_CONSUMERS; i++) {
//		pthread_join(consumers[i], NULL);
//	}
	for (int i = 0; i < num_consumers; ++i) {
		pthread_join(consumer[i], NULL);
	}

	// We are exiting, clean up
	delete[] consumer;
	delete[] args;
	delete[] buffer;
	delete emptySlots;
	delete fullSlots;
	pthread_mutex_destroy(&buf_mutex);

	printf("All threads have completed. Producer/Consumer simulation complete!\n");
	return EXIT_SUCCESS; // successful program termination!

}
