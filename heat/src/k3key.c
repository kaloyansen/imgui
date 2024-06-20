#include "k3key.h"

int mail()
{
     k3key key = k3keyi(5);
     key.on(&key, 2);
     key.dump(&key);
     key.on(&key, 3);
     key.dump(&key);
     return 0;
}

void k3key_init(void * arg, size_t isize)
{
     k3key * self = (k3key *)arg;
     self->size = isize <= 32 ? isize : 32;
     self->bit = 0;
}

void k3key_dump(void * arg)
{
     k3key * self = (k3key *)arg;
     fprintf(stdout, "k3key_dump\n");
     for (size_t i = 0; i < self->size; i ++) fprintf(stdout, "%zu %s\n", i, self->get(self, i) ? "true" : "false");
}

bool k3key_get(void * arg, size_t key)
{
     k3key * self = (k3key *)arg;
     if (key >= self->size) return false; // Out of bounds
     size_t bit_index = key % 32;
     return (self->bit >> bit_index) & 1;
}

void k3key_expo(void * arg, size_t key, bool valeur)
{
     k3key * self = (k3key *)arg;
     if (key >= self->size) return; // Out of bounds
     if (valeur) self->bit |= (1 << key);
     else self->bit &= ~(1 << key);
}

void k3key_on(void * arg, int key)
{
     k3key * self = (k3key *)arg;
     self->off(self);
     self->bit |= (1 << key);
}

void k3key_off(void * arg)
{
     k3key * self = (k3key *)arg;
     for (size_t i = 0; i < self->size; i ++) self->bit &= ~(1 << i);
}

void k3key_flip(void * arg, int key)
{
     k3key * self = (k3key *)arg;

     bool old = self->get(self, key);
     self->off(self);
     self->expo(self, key, !old);
}

k3key k3keyi(size_t isize)
{
     k3key obj;
     obj.size = isize;
     obj.init = k3key_init;
     obj.get  = k3key_get;
     obj.expo = k3key_expo;
     obj.on   = k3key_on;
     obj.flip = k3key_flip;
     obj.off  = k3key_off;
     obj.dump = k3key_dump;

     k3key_init(&obj, isize);
     return obj;
}

