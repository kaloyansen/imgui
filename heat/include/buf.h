#ifndef BUF_H
#define BUF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


#define BUFFER_SIZE 3e2
#define UPDATE_TIME 1e0
#define MAX_LINE_LENGTH 256
#define TEMP_SRC "sensors"
#define TEMP_TOKEN "Core 0"

typedef struct
{
     float * data;
     const char * token;
     float max;
     float min;
     float last;
     size_t size;
     size_t count;
     pthread_mutex_t mutex;
} CircularBuffer;


void * temperatureProducer(void *);
void initBuffer(CircularBuffer *, const char *);
void addFloatToBuffer(CircularBuffer *, float);
void processBuffer(CircularBuffer *);
void printBuffer(const CircularBuffer *);
void freeBuffer(CircularBuffer *);
void resetBuffer(CircularBuffer *);
float * histogram(const CircularBuffer *, int, float *, float *, float *);
float getCPUTemperature(const char *);
float readBuffer(CircularBuffer *, int);
void expandBuffer(CircularBuffer *);

#endif
