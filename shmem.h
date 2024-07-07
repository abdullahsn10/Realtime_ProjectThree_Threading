#ifndef __SHMEM_H_
#define __SHMEM_H_

#include "header.h"


int allocate_new_shmem(key_t key, size_t size){
    int shmid;
    // create shmem
    if ((shmid = shmget(key, size, IPC_CREAT | 0777)) == -1){
        perror("shmid -- parent -- creation\n");
        exit(2);
    }
    return shmid;
}

char* attach_to_shmem(int shmid){
    char* shmptr;
    if ((shmptr = (char *)shmat(shmid, 0, 0)) == (char *)-1)
    {
        perror("shmptr -- parent -- attach");
        exit(1);
    }
    return shmptr;
}

int connect_to_existing_shmem(key_t key){
    int shmid;
    if ((shmid = shmget(key, 0, 0)) == -1){
        perror("shmget -- producer -- access (producer get)");
        exit(2);
    }
    return shmid;
}

int clean_shmem(int shmid){
    shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0);
}


/*
    SHMEM COMPLETE EXAMPLE:
    ----------------------------------------------
    main application
    ----------------------------------------------
        // creating save storage shared memory between collecting and splitting
    // required data fields
    struct SAVE_STORAGE_SHMEM memory;
    static ushort start_val[2] = {1, 0};
    int semid, shmid;
    char *shmptr;
    union semun arg;
    key_t key_sv = ftok(".", SAVE_STORAGE_KEY);
    char key_passed[20];
    sprintf(key_passed, "%d", key_sv);

    struct SAVE_STORAGE_SHMEM *memptr;

    // create attach, and init shmem segment
    shmid = allocate_new_shmem(key_sv, sizeof(memory));
    shmptr = attach_to_shmem(shmid);

    // format shmem
    memcpy(shmptr, (char *)&memory, sizeof(memory));
    memptr = (struct SAVE_STORAGE_SHMEM *)shmptr;
    // initialize shmem values
    memptr->amount = 0; 
    memptr->num_of_dead_worker = 0;

    // create and init semaphore
    if ((semid = semget(key_sv, 2,
                        IPC_CREAT | 0777)) != -1)
    {
        arg.array = start_val;

        if (semctl(semid, 0, SETALL, arg) == -1)
        {
            perror("semctl -- parent -- initialization");
            exit(3);
        }
    }
    else
    {
        perror("semget -- parent -- creation");
        exit(4);
    }

    ---------------------------------------------------------------
    child application
    ---------------------------------------------------------------

    struct sembuf acquire = {0, -1, SEM_UNDO},
                release = {0, 1, SEM_UNDO};

    enum
    {
        COLLECTING_PROD,
        SPLITTING_CONS
    };
------------------------------------------
        int semid, shmid;
        pid_t ppid = getppid();
        char *shmptr;
        struct SAVE_STORAGE_SHMEM *memptr;
        key_t key_sv = atoi(argv[4]);


            // add its amount to save storage += amount
        // open the shaerd memory and add the new amount

        shmid = connect_to_existing_shmem(key_sv);
        shmptr = attach_to_shmem(shmid);
        memptr = (struct SAVE_STORAGE_SHMEM *)shmptr;


        if ((semid = semget(key_sv, 2, 0)) == -1)
        {
            perror("semget -- producer -- access");
            exit(3);
        }

        // Acquire
        acquire.sem_num = COLLECTING_PROD;
        if (semop(semid, &acquire, 1) == -1)
        {
            perror("semop -- producer -- acquire");
            exit(4);
        }
        // critical section
        memptr->amount += rcvd_msg_from_gnd.container.flour_amount_kg;
        memptr->num_of_dead_worker = num_of_dead_worker;
        printf("[Save Storage]: Total Amount = %d with dead worker %d\n", memptr->amount, memptr->num_of_dead_worker);

        // release
        release.sem_num = SPLITTING_CONS;
        if (semop(semid, &release, 1) == -1)
        {
            perror("semop -- producer -- release");
            exit(5);
        }

*/


#endif
