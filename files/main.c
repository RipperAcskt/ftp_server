#include "ring.h"
#include <unistd.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/sem.h>
#include <signal.h>

#define LEN_OF_BUFFER 260

typedef struct{
    int type;
    unsigned char message[LEN_OF_BUFFER];
}message;

void manufacturer();
void consumer();

int main(){
    pid_t pid[4];
    int p = 0;

    int shmid;
    int msqid;
    int semid;
    message ms;

    bool f = true;

    semid = semget (2001, 1, PERMS | IPC_CREAT);
    semctl(semid, 0, SETVAL, 1);
    
    msqid = msgget(10, PERMS | IPC_CREAT);

    shmid = shmget(202, sizeof(ring*), PERMS | IPC_CREAT);
    ring *ring_buffer = NULL;
    if((ring_buffer = (ring*)shmat(shmid, 0, 0)) == NULL) printf("erro1");

    init(ring_buffer, LEN_OF_BUFFER);

    while(f){
        fflush(stdin);
        switch(getc(stdin)){
            case '+':
                for(int i = 0; i < LEN_OF_BUFFER; i++) ms.message[i] = ring_buffer->buff[i];
                msgsnd(msqid, &ms, sizeof(message), 0);

                pid[p] = fork();
                if(!pid[p]) execv("manufacturer", NULL); 
                
                // for(int i = 0; i < LEN_OF_BUFFER; i++) printf("%d ", ring_buffer->buff[i]);
                // while(semctl(semid, 0, GETVAL, 0)){
                sleep(2);
                msgrcv(msqid, &ms, sizeof(message), 0, 0);
                ring_buffer->buff = malloc(LEN_OF_BUFFER*sizeof(char));
                for(int i = 0; i < LEN_OF_BUFFER; i++) ring_buffer->buff[i] = ms.message[i];
                p++;
                break;
            case '-':
                for(int i = 0; i < LEN_OF_BUFFER; i++) ms.message[i] = ring_buffer->buff[i];
                msgsnd(msqid, &ms, sizeof(message), 0);

                pid[p] = fork();
                if(!pid[p]) execv("consumer", NULL);;
                sleep(2);

                msgrcv(msqid, &ms, sizeof(message), 0, 0);
                ring_buffer->buff = malloc(LEN_OF_BUFFER*sizeof(char));
                for(int i = 0; i < LEN_OF_BUFFER; i++) ring_buffer->buff[i] = ms.message[i];
                p++;
                break;
            case 'q':
                for(int i = 0; i < 4; i++) kill(pid[i], SIGKILL);
                return 0;
        }
    }

    semctl (semid, 0, IPC_RMID, (struct semid_ds *) 0);

    msgctl(msqid, IPC_RMID, (struct msqid_ds*)0);

    shmctl (shmid, IPC_RMID, (struct shmid_ds *) 0);
    shmdt(ring_buffer);
    return 0;
}
