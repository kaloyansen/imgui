#ifndef K3KEY_H
#define K3KEY_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct k3key
{
     size_t size;
     uint32_t bit;

     void (* init)(void *, size_t);
     void (* dump)(void *);
     void (* on)(void *, int);
     void (* flip)(void *, int);
     void (* off)(void *);
     bool (* get)(void *, size_t);
     void (* expo)(void *, size_t, bool);
} k3key;

k3key k3keyi(size_t);
bool k3key_get(void *, size_t);
void k3key_expo(void *, size_t, bool);
void k3key_init(void *, size_t);
void k3key_on(void *, int);
void k3key_flip(void *, int);
void k3key_dump(void *);
void k3key_off(void *);

#endif
