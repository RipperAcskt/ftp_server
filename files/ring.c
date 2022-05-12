#include "ring.h"


int random_num(){
    struct timespec time_nano;
    clock_gettime(CLOCK_REALTIME, &time_nano);
    srand(time_nano.tv_nsec);
    return rand();
}

unsigned char* create_message(int size){
    unsigned char *message = malloc(size * sizeof(char));

    for(int i = 0; i < size; i++) message[i] = (unsigned char)random_num();

    return message;
}

int create_checksum(int size, unsigned char *message){
    int checksum = 0;

    for(int i = 0; i < size; i++) checksum += (unsigned char)message[i];

    return checksum%510;
}


void init(ring *ring_buffer, int size){

    ring_buffer->size = size;
    ring_buffer->buff = malloc(size * sizeof(unsigned char));
    ring_buffer->index_in = 0;
    ring_buffer->index_out = 0;
    ring_buffer->in = 0;
    ring_buffer->out = 0;
}

void put(ring *ring_buffer){
    unsigned char starting_line;
    int size, pre_hash;
    unsigned char *message, *hash;

    size = random_num() % 257;
    if(size == 256) starting_line = (unsigned char)128;
    else starting_line = (unsigned char)0;

    message = create_message(size);

    hash = malloc(2*sizeof(unsigned char));
    pre_hash = create_checksum(size, message);
    if(pre_hash > 255){
        hash[0] = (unsigned char)255;
        hash[1] = (unsigned char)pre_hash-255;
    }
    else{
        hash[0] = (unsigned char)pre_hash;
        hash[1] = (unsigned char)0;
    }

    if(size+4+ring_buffer->index_in > ring_buffer->size) {
        if(ring_buffer->in == ring_buffer->out) ring_buffer->index_in = 0;
        else {
            printf("%d %d\n",size+4+ring_buffer->index_in, ring_buffer->size);
            return;
        }
    }
    printf("starting line: %d\nhash: %d %d\nsize: %d\n", (int)starting_line, (int)hash[0], (int)hash[1], size);
    ring_buffer->buff[ring_buffer->index_in++] = starting_line;
    ring_buffer->buff[ring_buffer->index_in++] = hash[0];
    ring_buffer->buff[ring_buffer->index_in++] = hash[1];
    ring_buffer->buff[ring_buffer->index_in++] = (unsigned char)size;

    for(int i = 0; i < size; i++) ring_buffer->buff[ring_buffer->index_in++] = message[i];

    if(ring_buffer->index_in >= ring_buffer->size) ring_buffer->index_in = 0;
    ring_buffer->in++;
    printf("in: %d\n\n", ring_buffer->in);

    free(message);
    free(hash);

}

void pop(ring *ring_buffer){
    if(ring_buffer->in == ring_buffer->out) return;

    int size, checksum, starting_line;
    unsigned char *hash = malloc(2*sizeof(unsigned char));
    unsigned char *message;

    starting_line = (int)ring_buffer->buff[ring_buffer->index_out++];
    if(starting_line == 128) size = 256;
    else size = (int)ring_buffer->buff[ring_buffer->index_out+2];

    message = malloc(size*sizeof(unsigned char));

    hash[0] = ring_buffer->buff[ring_buffer->index_out++];
    hash[1] = ring_buffer->buff[ring_buffer->index_out++];
    ring_buffer->index_out++;

    for(int i = 0; i < size; i++) message[i] = ring_buffer->buff[ring_buffer->index_out++];

    checksum = create_checksum(size, message);
    if(checksum > 255){
        if((int)hash[0] != 255){
            printf("error of checksum!\n");
            return;
        }
        if(checksum-255 != (int)hash[1]){
            printf("error of checksum!\n");
            return;
        }
    }
    else{
        if(checksum != (int)hash[0]){
            printf("error of checksum!\n");
            return;
        }
    }

    printf("starting line: %d\nhash: %d %d\nsize: %d\n", starting_line, (int)hash[0], (int)hash[1], size);
    for(int i = 0; i < size; i++) printf("%d ", message[i]);
    printf("\n");

    if(ring_buffer->index_out >= ring_buffer->size) ring_buffer->index_out = 0;
    ring_buffer->out++;
    printf("out: %d\n\n", ring_buffer->out);

    free(hash);
    free(message);
}
