#ifndef __PILL_H
#define __PILL_H


#include "header.h"
#include "functions.h"
#include "msg_queue.h"
#include "shmem.h"
#include "random_lib.h"
#include "factory.h"
#include "pill.h"

// threads and thread arrays
pthread_t *foreign_employees;
pthread_t *inspecting_employees;
pthread_t *stopped_employees;
pthread_t *packaging_employees;
pthread_t production_thread;
pthread_t new_inspecting_employees[2];



// msg to main
struct SPEED_MESSAGE speed_message;

// prototypes
void production_simulation();
void *inspecting_func(void *arg);
void *packaging_func(void *arg);
void produce_plastic_container();
void stop_thread(int signo);
void continue_thread(int signo);
void create_new_thread(int signo);
void signal_stop(int signum);
void signal_continue(int signum);
void backing_foreign_employee(int signo);
int check_if_needed_new_thread();
void initialize_signals();
void add_to_foreign_array( pthread_t thread_id);


// line
struct PROD_LINE line;


// globals
int is_sent_employee = 0;
int foreign_count = 0;
int number_of_produced_plastic_container = 0;
int number_of_plastic_container_failed = 0;
int number_of_inspected_plastic_container_ = 0;
int queue_count = 0;
int inspected_queue_count = 0;
int number_of_packages = 0;
int number_of_passed_plastic_container = 0;
int inspecting_count = 0;
int stopped_count = 0;
int enable =0;
int counter_test = 5;
double test_amount_factor = 1.0;
int thread_id_uniq;
int is_init = 1;


// shared resources
struct PLASTIC_CONTAINER plastic_container_queue[QUEUE_SIZE];
struct PLASTIC_CONTAINER inspected_plastic_container_queue[QUEUE_SIZE];


// mutexes and condition variables
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_mutex_t inspected_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t inspected_not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t inspected_not_full = PTHREAD_COND_INITIALIZER;

// communction with opengl
int n, privatefifo, publicfifo;
struct message msg;

char PUBLIC_FIFO[30] = "/tmp/fifi";
char *fifo_fdes = "/tmp/fifi";

// file descriptor for fifo to main
int out_fd;



#endif