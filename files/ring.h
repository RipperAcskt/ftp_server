#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <time.h>

#define SHM_ID 2002
#define PERMS 0666
#define MAX_SIZE 260

typedef struct{
    unsigned char *buff;
    int index_in;
    int index_out;
    int size;
    int in;
    int out;
} ring;

void init(ring *ring_buffer, int size);
void put(ring *ring_buffer);
void pop(ring *ring_buffer);

int random_num();
int create_checksum(int size, unsigned char *message);
unsigned char* create_message(int size);
