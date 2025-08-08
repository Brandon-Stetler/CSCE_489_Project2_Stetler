# AFIT-CSCE489-PROJ2 

Author: Capt Brandon Stetler, AFIT Master's student

## Overview
This project implemented semaphores using the pthread mutex primitives, and then converted the single buffer storefront implementation found in babyyoda.cpp into a bounded buffer.

The problem is that there is one really busy toy store owner (producer thread), a storefront with N shelf locations, and a bunch of independent parents wanting to buy the latest Baby Yoda animatronic doll (consumer threads). The owner creates Baby Yoda dolls, numbered sequentially from 1 on up, out of the thin air and places each on one of the N locations in the storefront. The consumers snatch these items as soon as they are placed on the shelf (but no sooner) and announce to the world the serial number of the item they purchased.

To do this, the project implemented a `semaphore` class with `wait()` and `signal()` using `pthread_mutex_t` and `pthread_cond_t`.

## Repository Link 
https://github.com/Brandon-Stetler/CSCE_489_Project2_Stetler

## Build Instructions
```bash
make clean
make
```

## Usage
```bash
./babyyoda <buffersize> <num_consumers> <max_items>
```
- `buffersize`: The maximum number of items the store can hold
- `num_consumers`: Number of consumer threads
- `max_items`: The number of Baby Yoda dolls the producer should create before exiting

Example:
```bash
./babyyoda 5 15 10
```

## Project Files
- `Semaphore.h` & `Semaphore.cpp`: Semaphore implementation using pthreads
- `babyyoda.cpp`: Main simulation demonstrating bounded buffer of Yoda dolls with multiple consumers
- `Makefile`: Build instructions
- `README.md`: This file