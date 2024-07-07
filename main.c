#include "header.h"
#include "functions.h"
#include "constants.h"
#include "random_lib.h"
#include "msg_queue.h"
#include "factory.h"

//  globals
struct MEDICINE *medicines;
pid_t *production_lines;

char PUBLIC_FIFO[30] = "/tmp/fifi";
char *fifo_fdes = "/tmp/fifi";

// prototypes
void initialize_medicines();
void create_production_lines_processes();
void end_simulation();
void sort_by_remaining_medicine();
void add_or_update_speed_message(int thread_id, int remaining_medicine, int line_pid, int need_employee);
int compare_by_remaining_medicine(const void *a, const void *b);
void send_thread();
void send_signal_to_fast_line_and_reposition();
void delete_from_history_array();
void restore_employee(int line_pid);
void add_to_history_array(int source, int dest);


// communction with opengl
int n, privatefifo, publicfifo;
struct message msg;

// struct for the message
struct SPEED_MESSAGE message;
struct SPEED_MESSAGE *lines_speed_messages;

// history of lines' employees transactions
struct HISTORY *histroy_change;

// FIFO file descriptor
int fifo;
int speed_message_array_size = 0;
int array_of_history_size = 0;
int index_postion =0;

int main()
{
    // initialize signals
    initialize_signals();



    // read settings file
    read_settings_from_a_file("settings.txt");

    // initialize_sginaction();
    /* Open the public FIFO for writing */
    if ((publicfifo = open(PUBLIC, O_WRONLY)) == -1)
    {
        printf("Public error");
        perror(PUBLIC);
        exit(2);
    }

    // create different types of medicines
    initialize_medicines();

    // create fifo file for communication with production lines
    remove(fifo_fdes);
    if (mkfifo(fifo_fdes, __S_IFIFO | 0777) == -1)
    {
        perror("FIFO: Can't create the fifo\n");
        exit(50);
    }

    // set the alarm for the simulation time (in minutes)
    alarm(SIMULATION_TIME*60);

    // create production lines processes
    create_production_lines_processes();


    // open the FIFO file for reading
    fifo;
    if ((fifo = open(fifo_fdes, O_RDONLY | O_NONBLOCK)) == -1)
    {
        perror("FILE: Can't Open the FIFO file\n");
        exit(10);
    }

    while (1)
    {
   
        // Read the struct from the FIFO
        while (read(fifo, &message, sizeof(struct SPEED_MESSAGE)) > 0)
        {
            // printf("Recv:\tThread ID: %d, Remaining Medicine: %d\n", message.thread_id, message.reamining_medicine);
            add_or_update_speed_message(message.thread_id, message.reamining_medicine, message.line_pid, message.need_employee);
            sort_by_remaining_medicine();
            // NOTE.......
            if(message.need_employee == 1){
                send_thread();  
            }else if(message.restore_employee == 1){
                restore_employee(message.line_pid);
            }
        }
    }

    // function to end simulation
    end_simulation();
}


void initialize_signals()
{
    if (sigset(SIGALRM, end_simulation) == SIG_ERR)
    {
        perror("Sigset can not set SIGALRM");
        exit(SIGINT);
    }
}


void initialize_medicines()
{
    medicines = (struct MEDICINE *)malloc(sizeof(struct MEDICINE) * NUMBER_OF_DIFFERENT_MEDICINES);
    for (int i = 0; i < NUMBER_OF_DIFFERENT_MEDICINES; i++)
    {
        medicines[i].medicine_id = i + 1;
        medicines[i].medicine_type = (i % 2) == 0 ? 'L' : 'P';
    }
}

void create_production_lines_processes()
{

    production_lines = (pid_t *)malloc(sizeof(pid_t) * NUMBER_OF_PRODUCTION_LINES);
    pid_t pid; // temp var
    for (int i = 0; i < NUMBER_OF_PRODUCTION_LINES; i++)
    {
        if ((pid = fork()) == -1)
        {
            perror("Fork: Creation Error of lines\n");
            exit(5);
        }
        else if (pid == 0)
        {
            // child process - production lines

            char line_id[20];
            char line_speed[20];
            char medicine_id[20];
            sprintf(line_id, "%d", i + 1);
            sprintf(line_speed, "%d", get_random_number_in_range(6, 10));

            if (medicines[i % NUMBER_OF_DIFFERENT_MEDICINES].medicine_type == 'L')
            {
                // liquid production line
                sprintf(medicine_id, "%d", medicines[i % NUMBER_OF_DIFFERENT_MEDICINES].medicine_id);

                // send to openGL that a liquid production line is created for the medicine {id} with speed
                sprintf(msg.cmd_line, "A,%d,%d", atoi(line_id), atoi(line_speed));
                // send infomations to opengl creating new liquid production line
                write(publicfifo, (char *)&msg, sizeof(msg));
                execlp("./liquid_production_line", "liquid_production_line",
                       line_id, medicine_id, line_speed, NULL);

                // if success will not reach here
                perror("EXEC: can't execute production line liquid\n");
                exit(6);
            }
            else
            {
                // pill based production line
                sprintf(medicine_id, "%d", medicines[i % NUMBER_OF_DIFFERENT_MEDICINES].medicine_id);

                // send to openGL that a pill production line is created for the medicine {id} with speed
                // send pill productionLine
                sprintf(msg.cmd_line, "B,%d,%d", atoi(line_id), atoi(line_speed));
                // send infomations to opengl creating new liquid production line
                write(publicfifo, (char *)&msg, sizeof(msg));

                execlp("./pill_production_line", "pill_production_line",
                       line_id, medicine_id, line_speed, NULL);

                // if success will not reach here
                perror("EXEC: can't execute production line pill\n");
                exit(7);
            }
        }
        else
        {
            // parent process - main applicatiion
            production_lines[i] = pid;
        }
    }
}

// Function to get or update message
void add_or_update_speed_message(int thread_id, int remaining_medicine, int line_pid, int need_employee) {
    // Check if the line already exists in the array
    for (int i = 0; i < speed_message_array_size; i++) {
        if (lines_speed_messages[i].line_pid == line_pid) {
            // Update the remaining_medicine value if thread_id exists
            lines_speed_messages[i].reamining_medicine = remaining_medicine;
            lines_speed_messages[i].need_employee = need_employee;
            return;
        }
    }

    // If line id does not exist, add a new struct to the array
    speed_message_array_size += 1;
    lines_speed_messages = (struct SPEED_MESSAGE *)realloc(lines_speed_messages, speed_message_array_size * sizeof(struct SPEED_MESSAGE));

    // Check if memory reallocation was successful
    if (lines_speed_messages == NULL) {
        printf("Memory reallocation failed\n");
        exit(1);
    }

    // Initialize the new struct
    lines_speed_messages[speed_message_array_size - 1].thread_id = thread_id;
    lines_speed_messages[speed_message_array_size - 1].reamining_medicine = remaining_medicine;
    lines_speed_messages[speed_message_array_size - 1].line_pid = line_pid;
    lines_speed_messages[speed_message_array_size - 1].need_employee = need_employee;
}

// Comparison function for qsort
int compare_by_remaining_medicine(const void *a, const void *b)
{
    struct SPEED_MESSAGE *messageA = (struct SPEED_MESSAGE *)a;
    struct SPEED_MESSAGE *messageB = (struct SPEED_MESSAGE *)b;
    return messageA->reamining_medicine - messageB->reamining_medicine;
}

// Function to sort the array by remaining_medicine in ascending order
void sort_by_remaining_medicine()
{
    qsort(lines_speed_messages, speed_message_array_size, sizeof(struct SPEED_MESSAGE), compare_by_remaining_medicine);
}

// Function to end simulation
void end_simulation()
{
    printf("*******[THRESHOLD]: Simulation Time is ended*******\n");

    for (int i = 0; i < NUMBER_OF_PRODUCTION_LINES; i++)
    {
        kill(production_lines[i], SIGTERM);
    }
    free(medicines);
    free(production_lines);
    system("make clean");
    exit(0);
}


void send_thread()
{      
    // sent signal to fast line to get employee
    for (int i = 0; i < speed_message_array_size; i++)
        {   if(lines_speed_messages[i].need_employee == 1)
            {   
                // sent signal to fast line to get employees from it
                if(lines_speed_messages[0].reamining_medicine < 20){
                    // add the transaction to the history array (source, dest)
                    add_to_history_array(lines_speed_messages[0].line_pid, lines_speed_messages[i].line_pid);
                    send_signal_to_fast_line_and_reposition();
                    printf("Send signal to %d\n",lines_speed_messages[i].line_pid);
                    kill(lines_speed_messages[i].line_pid,SIGBUS);
                    lines_speed_messages[i].need_employee = 0;

                }
            }
        }
}
// Function to send signal to the fast line (index 0) and move it to the end
void send_signal_to_fast_line_and_reposition()
{
    if (speed_message_array_size == 0)
        return;


    // send signal to the fast line to stop 2 threads (employees)
    kill(lines_speed_messages[0].line_pid,SIGUSR1);


    // Move the first element to the end of the array
    sort_by_remaining_medicine();

    
}

// Function to add a new entry to the history array
void add_to_history_array(int source, int dest) {
    // If thread_id does not exist, add a new struct to the array
    array_of_history_size += 1;
    histroy_change = (struct HISTORY *)realloc(histroy_change, array_of_history_size * sizeof(struct HISTORY));

    // Check if memory reallocation was successful
    if (histroy_change == NULL) {
        printf("Memory reallocation failed\n");
        exit(1);
    }

    // Initialize the new struct
    histroy_change[array_of_history_size - 1].line_id_dest = dest;
    histroy_change[array_of_history_size - 1].line_id_source = source;
    histroy_change[array_of_history_size - 1].is_back = 0;
}

// Function to delete an entry from the history array
void delete_from_history_array() {
    for (int i = 0; i < array_of_history_size; i++) {
        if (histroy_change[i].is_back) {
            // Shift remaining elements to the left
            for (int j = i; j < array_of_history_size - 1; j++) {
                histroy_change[j] = histroy_change[j + 1];
            }
            array_of_history_size -= 1;
            histroy_change = (struct HISTORY *)realloc(histroy_change, array_of_history_size * sizeof(struct HISTORY));

            // Check if memory reallocation was successful
            if (histroy_change == NULL && array_of_history_size > 0) {
                printf("Memory reallocation failed\n");
                exit(1);
            }
        
            return; // Exit after deleting the first matching entry
        }
    }

}

void restore_employee(int line_pid)
{ // write for loop to history arra 

    for (index_postion = 0; index_postion < array_of_history_size; index_postion++)
    {
        if(histroy_change[index_postion].line_id_source == line_pid){
             // sent to proces to stop one of thread 
            printf("Restore employee to %d\n",histroy_change[index_postion].line_id_dest);
            kill(histroy_change[index_postion].line_id_dest,SIGINT);
            kill(line_pid,SIGUSR2);
            histroy_change[index_postion].is_back = 1;
            sleep(2);
        }
    }

    delete_from_history_array();

}

