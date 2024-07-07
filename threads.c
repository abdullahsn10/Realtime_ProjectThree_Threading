#include "header.h"


/*
    Example On Threads: Company Employees
*/

struct ARGS{
    int x;
    double y;
};





void* employee1_func(void* arg){
    int *my_id = (int*) arg;
    printf("Hello, Iam employee %d, Starting.....\n", *my_id);
    while(1){
        printf("I am doing Task 1\n");
        sleep(3);
        printf("Finished Task 1\n");
        sleep(2);
    }


}

void* employee2_func(void* arg){
    int *my_id = (int*) arg;
    printf("Hello, Iam employee %d, Starting.....\n", *my_id);
    while(1){
        printf("I am doing Task 2\n");
        sleep(3);
        printf("Finished Task 2\n");
        sleep(2);
    }

}

void* test_args_func(void* arg){
    struct ARGS* data = (struct ARGS*) arg;
    printf("Data x = %d\n", data->x);
    printf("Data y = %lf\n", data->y);
    pthread_exit((void*) 3);
}



// threads
pthread_t emp1, emp2, test_th;
int emp1_id = 10, emp2_id = 20;

int main(int argc, char** argv){

    struct ARGS args;
    args.x = 100;
    args.y = 55.5;

    // creating threads
    pthread_create(&emp1, NULL, employee1_func, (void*) &emp1_id);
    pthread_create(&emp2, NULL, employee2_func, (void*) &emp2_id);
    pthread_create(&test_th, NULL, test_args_func, (void*) &args);


    // wait threads to finish
    pthread_join(emp1, NULL);
    pthread_join(emp2, NULL);
    pthread_join(test_th, NULL);

    

    exit(0);

}