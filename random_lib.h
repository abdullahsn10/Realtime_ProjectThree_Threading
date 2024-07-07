#ifndef __SIM_H_
#define __SIM_H_
#include "header.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
// Function to generate a unique seed
unsigned int generate_seed() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (unsigned int)(tv.tv_sec ^ tv.tv_usec ^ getpid());
}

int get_random_number_in_range( int min, int max);


// Function to generate a random number in the range [min, max]
int get_random_number_in_range( int min, int max) {
    srand(generate_seed());
    return (rand() % (max - min + 1)) + min;
}
int get_true_or_false_with_probability( int min, int max,int value) {
    int prob= get_random_number_in_range(min, max);
    if (prob > value)
    {
        return 0;
    }

    return 1;
}



#endif