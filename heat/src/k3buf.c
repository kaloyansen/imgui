#include "k3buf.h"

static void initBuffer(k3buf * cb)
{
     cb->data = NULL;
     cb->min = 1000;
     cb->max = 0;
     cb->size = 0;
     cb->count = 0;
     cb->last = 0;
     pthread_mutex_init(&cb->mutex, NULL);
     printf("%s initialized\n", cb->token);
}

static void expandBuffer(k3buf * cb)
{
     cb->size ++;
     float * array = (float *)realloc(cb->data, cb->size * sizeof(float));
     if (array == NULL) printf("allocation error in function expandBuffer\n");
     else cb->data = array; 
}

static void minmaxBuffer(k3buf * cb)
{
     float min = 1e8;
     float max = 0;
     for (size_t i = 1; i < cb->size; ++i)
     {
          float val = cb->data[i];
          max = max < val ? val : max;
          min = min > val ? val : min;
     }

     cb->min = min;
     cb->max = max;
}

static void addFloatToBuffer(k3buf * cb, float value)
{

     pthread_mutex_lock(&cb->mutex);

     minmaxBuffer(cb);
     if (cb->size < BUFFER_SIZE) expandBuffer(cb);
     else for (size_t i = 1; i < cb->size; ++i) cb->data[i - 1] = cb->data[i];
     cb->data[cb->size - 1] = cb->last = value;
     cb->count ++;

     pthread_mutex_unlock(&cb->mutex);
}

static float getMotherfucker(const char * cmd, const char * token)
{
     FILE *fp;
     char path[MAX_LINE_LENGTH];
     float temperature = 0.0;
     int found = 0;

     fp = popen(cmd, "r");
     if (fp == NULL)
     {
          printf("popen failed\n");
          perror("popen failed");
          return -1;
     }

     while (fgets(path, MAX_LINE_LENGTH, fp) != NULL)
     {
          if (strstr(path, token) != NULL)
          {
               char *temp_str = strstr(path, "+");
               if (temp_str != NULL)
               {
                    sscanf(temp_str + 1, "%f", &temperature);
                    found = 1;
                    break;
               }
          }
     }

     pclose(fp);

     if (!found)
     {
          fprintf(stderr, "Temperature not found\n");
          return -1;
     }
     return temperature;
}

void * k3buf_work(void * arg)
{
     k3buf * self = (k3buf *)arg;
     while (1)
     {
          float temp = getMotherfucker("sensors", self->token);
          //printf("k3_work: %f\n", temp);
          if (temp > 0) addFloatToBuffer(self, temp);
          sleep(UPDATE_TIME);
     }
     return NULL;
}

void k3buf_die(void * arg)
{
     k3buf * self = (k3buf *)arg;
     if (self)
     {
          if (self->data)
          {
               free(self->data);
               self->data = NULL;
          }
          self->count = 0;
          self->size = 0;
     }
}

void k3buf_print(void * arg)
{
     k3buf * self = (k3buf *)arg;
     printf("\e[1;1H\e[2J");
     printf("Buffer: ");
     for (size_t i = 0; i < self->size; i++)
     {
          int index = (int)i;
          float fata = self->data[index];
          printf("%5.1f ", fata);
     }
     printf("\n");
}

void k3buf_reset(void * arg)
{
     k3buf * self = (k3buf *)arg;
     pthread_mutex_lock(&self->mutex);

     if (self->size != 0) self->die(self);
     
     pthread_mutex_unlock(&self->mutex);
     return;
}

k3buf k3bufi(const char * token)
{
     k3buf obj;
     obj.token = token;
     obj.die   = k3buf_die;
     obj.print = k3buf_print;
     obj.reset = k3buf_reset;
     obj.work  = k3buf_work;
     initBuffer(&obj);
     return obj;
}

