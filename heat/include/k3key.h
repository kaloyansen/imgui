#ifndef K3KEY_H
#define K3KEY_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct k3key
{
     int size;
     bool * val;

     void (* init)(struct k3key *, int);
     void (* die)(struct k3key *);
     void (* dump)(struct k3key *);
     void (* on)(struct k3key *, int);
     void (* flip)(struct k3key *, int);
     void (* off)(struct k3key *);
     bool * (* get)(struct k3key *, int);
} k3key;

void k3key_init(k3key * self, int isize);
void k3key_die(k3key * self);
void k3key_dump(k3key * self);
void k3key_on(k3key * self, int key);
void k3key_flip(k3key * self, int key);
void k3key_off(k3key * self);
bool * k3key_get(k3key * self, int key);
k3key create_k3key(int isize);

#endif
