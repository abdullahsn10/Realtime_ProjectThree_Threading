#ifndef __MSGQ_H_
#define __MSGQ_H_
#include "header.h"

// typedef struct{
//     long msg_type;
//     char msg_data[50];
// } MESSAGE;


key_t create_unique_key(char* path, int id){
    return ftok(path, id);
}


int create_MQ(key_t key){

    int mid;
    if((mid = msgget(key, IPC_CREAT | 0777)) == -1){
        perror("Creation of the msg Q error\n");
        exit(66);
    }
    return mid;
}

void clean_MQ(int mid){
    msgctl(mid, IPC_RMID, (struct msqid_ds *) 0);
}

int check_queue_empty(int mid, long msg_type)
{
    if (msgrcv(mid, NULL, 0, msg_type, IPC_NOWAIT) == -1)
    {
        if (errno == E2BIG)
            return 0; // there is data
    }

    return 1; // empty queue
}

/*
    Send and Recv Example
--------------------------------------------------------------
    // msg rcvd from plane
    struct PLANE_TO_GND_MSG msg;

    // msg to be sent to collecting mq
    struct GND_TO_COLLECT_MSG msg_to_collect;

    // recv info and store in msg
    if(msgrcv(plane_to_gnd_mqid, &msg, sizeof(msg), 20, 0) == -1){
            perror("Recv Error\n");
            exit(70);
    }

    // send this container to the gnd-collecter msg queue
    msg_to_collect.msg_type = 50;
    msg_to_collect.container = on_ground_container[container_index];
    if(msgsnd(gnd_to_collect_mqid, &msg_to_collect, sizeof(msg_to_collect), IPC_NOWAIT) == -1){
        perror("Collecting Queue send error\n");
        exit(75);
    }

*/










#endif