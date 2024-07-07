#include "header.h"
#include "functions.h"
#include "msg_queue.h"
#include "shmem.h"
#include "random_lib.h"
#include "factory.h"
#include <signal.h>
#include "liquid.h"



int main(int argc, char **argv)
{

    // check args
    if (argc != 4)
    {
        perror("Liquid Usage: not enough args\n");
        exit(10);
    }

    /* Open the public FIFO for writing */
    if ((publicfifo = open(PUBLIC, O_WRONLY)) == -1)
    {
        printf("Public error");
        perror(PUBLIC);
        exit(2);
    }

    // read settings from the settings file
    read_settings_from_a_file("liquid_settings.txt");

    // get the number of inspecting and packaging employees
    initialize_workers_numbers();

    // initialize signals
    initialize_signals();

    // msg to main 
    // save the process id in the speed_message struct
    speed_message.line_pid = getpid();
    speed_message.need_employee = 0;
    speed_message.restore_employee = 0;



    // get args
    line.line_id = atoi(argv[1]);
    line.produced_medicine_id = atoi(argv[2]);
    line.speed_of_line = atoi(argv[3]);

    // initialize an array to save stopped employees data
    stopped_employees = (pthread_t *)malloc(sizeof(pthread_t) * NUMBER_OF_INSPECTING_WORKERS);

    // create inspecting employees
    inspecting_employees = (pthread_t *)malloc(sizeof(pthread_t) * NUMBER_OF_INSPECTING_WORKERS);


    for (int i = 0; i < NUMBER_OF_INSPECTING_WORKERS; i++)
    {
        inspecting_count++;

        // create inspecting thread and pass the id of the thread
        pthread_create(&inspecting_employees[i], NULL, inspecting_func, NULL);
        
        // send to openGL that an inspecting worker is created
        // send liquid productionLine create new inspecting employee
        sprintf(msg.cmd_line, "C,%d,%d", line.line_id, line.speed_of_line);
        write(publicfifo, (char *)&msg, sizeof(msg));
    }

    // create packaging employees
    packaging_employees = (pthread_t *)malloc(sizeof(pthread_t) * NUMBER_OF_PACKAGING_WORKERS);
    for (int i = 0; i < NUMBER_OF_PACKAGING_WORKERS; i++)
    {
        pthread_create(&packaging_employees[i], NULL, packaging_func, NULL);
        // send to openGL that an packaging worker is created
        // send liquid productionLine create new packging employee
        sprintf(msg.cmd_line, "D,%d,%d", line.line_id, line.speed_of_line);
        write(publicfifo, (char *)&msg, sizeof(msg));
    }


    // open fifo
    out_fd = open(fifo_fdes, O_WRONLY);
    if (out_fd == -1)
    {
        perror("open error");
        exit(0);
    }

    production_simulation();

    // join threads
    for (int i = 0; i < NUMBER_OF_INSPECTING_WORKERS; i++)
    {
        pthread_join(inspecting_employees[i], NULL);
    }

    for (int i = 0; i < NUMBER_OF_PACKAGING_WORKERS; i++)
    {
        pthread_join(packaging_employees[i], NULL);
    }

    exit(0);
}


// Function to simulate the production of bottles
void production_simulation()
{
    // Producing bottles
    while (number_of_produced_bottles < THRESHOLD_OF_NEEDED_LIQUID_MEDICINES)
    {

        // create a set of bottles according speed of the line
        produce_bottle();
        // wake up all inspecting workers that are waiting for the queue to be not empty
        pthread_cond_broadcast(&not_empty);
        // simulate production time of the line
        sleep(get_random_number_in_range(1, 2) / 2);
    }
}


// Function to produce a bottle
void produce_bottle()
{

    // acquire
    pthread_mutex_trylock(&queue_mutex);

    // critical section
    // create a new bottle
    struct BOTTLE new_bottle = {
        .bottle_id = number_of_produced_bottles + 1,
        .medicine_id = line.produced_medicine_id,
        .has_normal_liquid_level = get_true_or_false_with_probability(0, 200, 195),
        .has_correct_color = get_true_or_false_with_probability(0, 200, 195),
        .is_sealed = get_true_or_false_with_probability(0, 200, 195),
        .has_correct_label = get_true_or_false_with_probability(0, 200, 195),
        .has_correct_expiry_date = get_true_or_false_with_probability(0, 200, 195)};

    bottle_queue[queue_count] = new_bottle;
    queue_count++;
    number_of_produced_bottles++;
    // send to openGL that a bottle is produced
    // send to openGL that value of produced bottles
    sprintf(msg.cmd_line, "G,%d,%d", line.line_id, number_of_produced_bottles);
    write(publicfifo, (char *)&msg, sizeof(msg));

    // release
    pthread_mutex_unlock(&queue_mutex);
}

void *inspecting_func(void *arg)
{

    // set the signal handler for the thread
    signal(SIGRTMIN, signal_stop);
    signal(SIGRTMAX, signal_continue);


    thread_id_uniq = pthread_self();

    while (1)
    {
        if(enable == 1){
            thread_id_uniq = pthread_self();
            enable = 0;
        }

        // acquire
        pthread_mutex_lock(&queue_mutex);

        while (queue_count == 0)
        {
            // if the queue is empty, then wait
            pthread_cond_wait(&not_empty, &queue_mutex);
        }

        // critical section

        // get the bottle
        struct BOTTLE bottle = bottle_queue[queue_count - 1];
        queue_count--;
        number_of_inspected_bottles++;
        // send to openGL that a bottle is inspected
        // send to openGL that value of inspected bottles
        sprintf(msg.cmd_line, "H,%d,%d", line.line_id, number_of_inspected_bottles);
        write(publicfifo, (char *)&msg, sizeof(msg));

        // pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&queue_mutex);

        sleep(get_random_number_in_range(1, 4));

        // inspect bottle
        if (bottle.has_normal_liquid_level && bottle.has_correct_color &&
            bottle.is_sealed && bottle.has_correct_label && bottle.has_correct_expiry_date)
        {
            number_of_passed_bottles++;

            // send to openGL that a bottle is passed
            sprintf(msg.cmd_line, "I,%d,%d", line.line_id, number_of_passed_bottles);
            write(publicfifo, (char *)&msg, sizeof(msg));

            // add the inspected bottle to inspected_bottle_queue
            // acquire lock
            pthread_mutex_lock(&inspected_queue_mutex);

            while (inspected_queue_count == QUEUE_SIZE)
            {
                pthread_cond_wait(&inspected_not_full, &inspected_queue_mutex);
            }

            // critical section
            inspected_bottle_queue[inspected_queue_count] = bottle;
            inspected_queue_count++;

            pthread_cond_signal(&inspected_not_empty);
            pthread_mutex_unlock(&inspected_queue_mutex);
        }
        else
        {
            // Bottle failed inspection
            // send to openGL that a bottle is failed

            number_of_bottles_failed++;
            sprintf(msg.cmd_line, "J,%d,%d", line.line_id, number_of_bottles_failed);
            write(publicfifo, (char *)&msg, sizeof(msg));
        }

        // simulate inspection time
        sleep(get_random_number_in_range(1, 2));

        // check if a new inspecting employee is needed
        int self = pthread_self();
        if (thread_id_uniq == self )
        {
            // check if new employee is needed, then create a new inspecting employee if 
            // we didn't send a new employee before
            // if we sent, then restore the employee
            int need_employee = check_if_needed_new_thread();
            if( need_employee && is_sent_employee){
                speed_message.restore_employee = need_employee;

            }else if(need_employee){
                speed_message.need_employee = 1;
            }

            speed_message.reamining_medicine = number_of_produced_bottles - number_of_inspected_bottles;

            // send the speed speed_message to the main appliaction
            if (write(out_fd, &speed_message, sizeof(struct SPEED_MESSAGE)) == -1)
            {
                perror("sent error");
                exit(0);
            }
            speed_message.need_employee = 0;
            speed_message.restore_employee = 0;
        }


    }
}

void *packaging_func(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&inspected_queue_mutex);

        while (inspected_queue_count == 0)
        {
            pthread_cond_wait(&inspected_not_empty, &inspected_queue_mutex);
        }

        struct BOTTLE bottle = inspected_bottle_queue[inspected_queue_count - 1];
        inspected_queue_count--;
        pthread_cond_signal(&inspected_not_full);
        pthread_mutex_unlock(&inspected_queue_mutex);

        // Simulate packaging process
        struct BOTTLE_PACKAGE package;
        number_of_packages++;

        // Send to openGL that a bottle is packaged
        // TODO:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        // send to openGl number of packages in production line of liquid
        sprintf(msg.cmd_line, "K,%d,%d", line.line_id, number_of_packages);
        write(publicfifo, (char *)&msg, sizeof(msg));

        package.package_id = number_of_packages;
        package.bottle = bottle;
        package.folded_perspection = "Prescribed medication details";

        // Simulate time taken to package the bottle
        sleep(1 / line.speed_of_line);

        // send to openGL total number of packages
        // TODO:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        // send to openGl number of packages in production line of liquid
        sprintf(msg.cmd_line, "L,%d,%d", line.line_id, number_of_packages);
        write(publicfifo, (char *)&msg, sizeof(msg));
    }

    pthread_exit(NULL);
}

void initialize_signals()
{
    if (sigset(SIGUSR1, stop_thread) == SIG_ERR)
    {
        perror("Sigset can not set SIGALRM");
        exit(SIGINT);
    }
    if (sigset(SIGUSR2, continue_thread) == SIG_ERR)
    {
        perror("Sigset can not set SIGALRM");
        exit(SIGINT);
    }
    if (sigset(SIGBUS, create_new_thread) == SIG_ERR)
    {
        perror("Sigset can not set SIGALRM");
        exit(SIGINT);
    }
        if (sigset(SIGINT, backing_foreign_employee) == SIG_ERR)
    {
        perror("Sigset can not set SIGALRM");
        exit(SIGINT);
    }
}


void stop_thread(int signo)
{
    
    pthread_mutex_unlock(&queue_mutex);
    if (inspecting_count > 2)
    {   
        is_sent_employee=1;

        // Stop the thread at index 0 in inspecting_employees
        pthread_kill(inspecting_employees[0], SIGRTMIN);
        pthread_kill(inspecting_employees[1], SIGRTMIN);

        // Move the stopped thread to stopped_employees array
        stopped_employees[stopped_count++] = inspecting_employees[0];
        stopped_employees[stopped_count++] = inspecting_employees[1];


        // shift inspecting employees 2 steps to the left
        for (int i = 2; i < inspecting_count; i++)
        {
            inspecting_employees[i - 2] = inspecting_employees[i];
        }

        // Decrement the count of inspecting employees
        inspecting_count -= 2;

        printf("line %d stop thread new counter %d\n", line.line_id, inspecting_count);

    }

}


void backing_foreign_employee(int signo)
{
    
    pthread_mutex_unlock(&queue_mutex);
    

        // Stop the thread at index 0 in inspecting_employees
        pthread_kill(foreign_employees[0], SIGRTMIN);
        pthread_kill(foreign_employees[1], SIGRTMIN);


        // shift inspecting employees 2 steps to the left
        for (int i = 2; i < foreign_count; i++)
        {
            foreign_employees[i - 2] = foreign_employees[i];
        }

        // Decrement the count of inspecting employees
        foreign_count -= 2;

}





















void continue_thread(int signo)
{

    pthread_mutex_unlock(&queue_mutex);

    if (stopped_count > 1)
    {
        // Continue the thread at index 0 in stopped_employees
        pthread_kill(stopped_employees[0], SIGRTMAX);
        pthread_kill(stopped_employees[1], SIGRTMAX);

        // Move the continued thread back to inspecting_employees array
        inspecting_employees[inspecting_count++] = stopped_employees[0];
        inspecting_employees[inspecting_count++] = stopped_employees[1];

        // Remove the thread from stopped_employees array
        for (int i = 2; i < stopped_count; i++)
        {
            stopped_employees[i - 2] = stopped_employees[i];
        }

        // Decrement the count of stopped employees
        stopped_count-=2;

        foreign_employees = (pthread_t *)realloc(foreign_employees, foreign_count * sizeof(pthread_t));
    }

}
void create_new_thread(int signo)
{
    printf("the line %d get new thread \n", line.line_id);
    fflush(stdout);
    pthread_t new_inspecting;
    pthread_create(&new_inspecting, NULL, inspecting_func, NULL);
    add_to_foreign_array(new_inspecting);
    pthread_t new_inspecting1;
    pthread_create(&new_inspecting1, NULL, inspecting_func, NULL);
    add_to_foreign_array(new_inspecting1);
    // change the need employee to 0
    speed_message.need_employee = 0;

    sprintf(msg.cmd_line, "C,%d,%d", line.line_id, line.speed_of_line);
    write(publicfifo, (char *)&msg, sizeof(msg));
    sprintf(msg.cmd_line, "C,%d,%d", line.line_id, line.speed_of_line);
    write(publicfifo, (char *)&msg, sizeof(msg));
    
}
int check_if_needed_new_thread()
{
    // check the gap between produced and inspected bottles
    // if the gap is more than 20 bottles, then create a new inspecting employee
    // in order to decrease the gap and speedup the inspection process
    // counter_test >= 5 &&
    if (counter_test >= 5 && (number_of_produced_bottles - number_of_inspected_bottles) > (30 * test_amount_factor))
    {
        // create a new 3 inspecting employees
        counter_test = 0;
        test_amount_factor += 0.1;

        return 1;
    }
    counter_test++;
    return 0;
    
}


void signal_stop(int signum)
{   
    int thread_id = pthread_self();

    if(thread_id == thread_id_uniq){
            enable = 1;
    }

    // free the mutex if lock by this thread
    pthread_mutex_unlock(&queue_mutex);
    pthread_mutex_unlock(&inspected_queue_mutex);

    printf("Received signal %d in thread %lu\n", line.line_id, pthread_self());
    fflush(stdout);

    // reduce the value in opneGL of the number of inspecting employees in liquid line
    sprintf(msg.cmd_line, "S,%d,%d", line.line_id, line.speed_of_line);
    write(publicfifo, (char *)&msg, sizeof(msg));

    pause();

}


void signal_continue(int signum)
{   
    is_sent_employee=0;

    sprintf(msg.cmd_line, "C,%d,%d", line.line_id, line.speed_of_line);
    write(publicfifo, (char *)&msg, sizeof(msg));
    printf("continue  signal %d in thread %lu\n", signum, pthread_self());
    fflush(stdout);
}

void add_to_foreign_array( pthread_t thread_id) {
    // If thread_id does not exist, add a new struct to the array
    foreign_count += 1;
    foreign_employees = (pthread_t *)realloc(foreign_employees, foreign_count * sizeof(pthread_t));

    // Check if memory reallocation was successful
    if (foreign_employees == NULL) {
        printf("Memory reallocation failed\n");
        exit(1);
    }

    // Initialize the new struct
    foreign_employees[foreign_count - 1] = thread_id;

}