#include "ring.h"
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <errno.h>
#include <unistd.h>

#define LEN_OF_BUFFER 260

typedef struct{
    int type;
    unsigned char message[LEN_OF_BUFFER];
}message;

int main(){ 
    // printf("ok\n");   
    int shmid;
    int msqid;
    int semid;
    message ms;

    ring *ring_buffer;
    if((shmid = shmget(202, sizeof(ring*), 0)) < 0){ printf("error2 %d %d\n", shmid, errno); exit(0);}
        
    if((ring_buffer = (ring*)shmat(shmid, 0, 0)) == NULL) printf("error3");

    semid = semget (2001, 1, 0);
    // printf("ok\n");
    if(semctl(semid, 0, GETVAL, 0)) exit(0);

    while(1){
        // printf("ok\n");
        semctl(semid, 0, SETVAL, 0);
        msqid = msgget(10, 0);

        msgrcv(msqid, &ms, sizeof(message), 0, 0);
        
        
        

        ring_buffer->buff = malloc(LEN_OF_BUFFER * sizeof(unsigned char));

        for(int i = 0; i < LEN_OF_BUFFER; i++) ring_buffer->buff[i] = ms.message[i];


        put(ring_buffer);
        // printf("ok\n");
        semctl(semid, 0, SETVAL, 1);
        
        for(int i = 0; i < LEN_OF_BUFFER; i++) ms.message[i] = ring_buffer->buff[i];
        // printf("ok\n");
        msqid = msgget(10, PERMS);
        // printf("%d\n", msqid);
        if(msgsnd(msqid, &ms, sizeof(message), 0) == -1){
            printf("message error %d\n", errno);
            exit(1);
        }
        // printf("sleep\n");
        sleep(5);
    }
    
    shmdt(ring_buffer);
    exit(0);
}

