#ifndef K3KEY_H
#define K3KEY_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct k3key
{
     int size;
     bool * val;

     void (* init)(void *, int);
     void (* die)(void *);
     void (* dump)(void *);
     void (* on)(void *, int);
     void (* flip)(void *, int);
     void (* off)(void *);
     bool * (* get)(void *, int);
} k3key;

k3key k3keyi(int);
bool * k3key_get(void *, int);
void k3key_init(void *, int);
void k3key_on(void *, int);
void k3key_flip(void *, int);
void k3key_die(void *);
void k3key_dump(void *);
void k3key_off(void *);

#endif
