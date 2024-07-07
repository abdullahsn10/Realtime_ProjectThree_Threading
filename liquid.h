#ifndef LIQUID_H
#define LIQUID_H

#include "header.h"
#include "functions.h"
#include "msg_queue.h"
#include "shmem.h"
#include "random_lib.h"
#include "factory.h"
#include <signal.h>




// globals
struct PROD_LINE line; // production line

// threads and thread arrays
pthread_t *inspecting_employees;
pthread_t *packaging_employees;
pthread_t production_thread;
pthread_t *stopped_employees;
pthread_t *foreign_employees;
// counters and flags
int number_of_produced_bottles = 0;
int number_of_inspected_bottles = 0;
int number_of_packages = 0;
int number_of_bottles_failed = 0;
int number_of_produced_bottles_group = 0;
int number_of_passed_bottles = 0;
int inspecting_count = 0;
int stopped_count = 0;
int enable = 0;
int foreign_count = 0;

// shared resources
struct BOTTLE bottle_queue[QUEUE_SIZE];
struct BOTTLE inspected_bottle_queue[QUEUE_SIZE];
int queue_count = 0;
int inspected_queue_count = 0;

// mutexes and conditions
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_mutex_t inspected_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t inspected_not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t inspected_not_full = PTHREAD_COND_INITIALIZER;

// new inspecting employees threads
pthread_t new_inspecting_employees[2];


// msg to main
struct SPEED_MESSAGE speed_message;

// main thread id
pthread_t main_thread;

int is_init = 1;


// prototypes
void *inspecting_func(void *arg);
void *packaging_func(void *arg);
void *production_line(void *arg);
void produce_bottle();
void initialize_signals();
void stop_thread(int signo);
void continue_thread(int signo);
void create_new_thread(int signo);
int check_if_needed_new_thread();
int check_random_liquid_level();
void signal_stop(int signum);
void signal_continue(int signum);
void backing_foreign_employee(int signo);
void add_to_foreign_array( pthread_t thread_id) ;

// communction with opengl
int n, privatefifo, publicfifo;
struct message msg;
char PUBLIC_FIFO[30] = "/tmp/fifi";
char *fifo_fdes = "/tmp/fifi";

// communication with main
int out_fd;

int thread_id_uniq;
int counter_test = 5;
double test_amount_factor = 1.0;
int is_sent_employee = 0;

#endif