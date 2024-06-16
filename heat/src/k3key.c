#include "k3key.h"

void k3key_init(k3key * self, int isize)
{
     self->size = isize;
     self->val = (bool *)calloc(false, isize * sizeof(bool));
     if (!self->val) fprintf(stderr, "init failed\n");
}

void k3key_die(k3key * self)
{
     if (self->val) free((void *)self->val);
     else fprintf(stderr, "cannot die\n");
}

void k3key_dump(k3key * self)
{
     fprintf(stdout, "k3key_dump\n");
     for (int i = 0; i < self->size; i ++)
     {
          bool * p = self->get(self, i);
          fprintf(stdout, "%d %s\n", i, *p ? "true" : "false");
     }
}

void k3key_off(k3key * self)
{
     for (int i = 0; i < self->size; i ++)
     {
          bool * p = self->get(self, i);
          *p = false;
     }
}

bool * k3key_get(k3key * self, int key)
{
     return self->val + key;
}


void k3key_on(k3key * self, int key)
{
     self->off(self);
     bool * p = self->get(self, key);
     *p = true;
}

void k3key_flip(k3key * self, int key)
{
     bool * p = self->get(self, key);
     bool old = *p;
     self->off(self);
     *p = !old;
}

k3key create_k3key(int isize)
{
     k3key obj;
     obj.size = isize;
     obj.init = k3key_init;
     obj.die = k3key_die;
     obj.get = k3key_get;
     obj.on = k3key_on;
     obj.flip = k3key_flip;
     obj.off = k3key_off;
     obj.dump = k3key_dump;

     k3key_init(&obj, isize);
     return obj;
}


/*

int main()
{
     k3key key = create_k3key(5);
     key.on(&key, 2);
     key.dump(&key);
     key.on(&key, 3);
     key.dump(&key);

     key.die(&key);
     
     return 0;
}
*/
