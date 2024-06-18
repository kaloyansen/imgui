#ifndef K3BUF_H
#define K3BUF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 3e2
#define UPDATE_TIME 1e0
#define MAX_LINE_LENGTH 256

typedef struct k3buf
{
     pthread_mutex_t mutex;
     const char * token;
     float * data;
     size_t size;
     size_t count;
     float max;
     float min;
     float last;

     void * (* work)(void *);
     void (* print)(void *);
     void (* reset)(void *);
     void (* die)(void *);
} k3buf;

k3buf k3bufi(const char *);
void * k3buf_work(void *);
void k3buf_print(void *);
void k3buf_reset(void *);
void k3buf_die(void *);

#endif
