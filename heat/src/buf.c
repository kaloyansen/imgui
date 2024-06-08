#include "buf.h"

void initBuffer(CircularBuffer * cb, const char * token)
{
     cb->token = token;
     cb->data = NULL;
     cb->min = 1000;
     cb->max = 0;
     cb->size = 0;
     cb->count = 0;
     cb->last = 0;
     pthread_mutex_init(&cb->mutex, NULL);
     printf("%s initialized\n", token);
}

void expandBuffer(CircularBuffer * cb)
{
     cb->size ++;
     float * array = (float *)realloc(cb->data, cb->size * sizeof(float));
     if (array == NULL) printf("allocation error in function expandBuffer\n");
     else cb->data = array; 
}


void minmaxBuffer(CircularBuffer * cb)
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

void addFloatToBuffer(CircularBuffer * cb, float value)
{

     pthread_mutex_lock(&cb->mutex);

     minmaxBuffer(cb);
     if (cb->size < BUFFER_SIZE) expandBuffer(cb);
     else for (size_t i = 1; i < cb->size; ++i) cb->data[i - 1] = cb->data[i];
     cb->data[cb->size - 1] = cb->last = value;
     cb->count ++;

     pthread_mutex_unlock(&cb->mutex);
}


float * histogram(const CircularBuffer * cb, int histo_size, float * bin_min, float * bin_max, float * hsize)
{
     float * x = (float*)malloc(histo_size * sizeof(float));
     for (int i = 0; i < histo_size; i ++) x[i] = 0;

     float bin_width = (cb->max - cb->min) / histo_size;
     *bin_max = 0;
     *bin_min = 1e10;
     *hsize = histo_size;
     
     for (size_t i = 0; i < cb->size; i ++)
     {
          float value = cb->data[i];
          int bin = (int)((value - cb->min) / bin_width);
          if (bin >= 0 && bin < histo_size)
          {
               //float old_value = x[bin];
               x[bin] = x[bin] + 1;
               *bin_min = *bin_min < x[bin] ? *bin_min : x[bin];
               *bin_max = *bin_max > x[bin] ? *bin_max : x[bin];
          }
               
     }
     return x;
}


void resetBuffer(CircularBuffer * cb)
{
     pthread_mutex_lock(&cb->mutex);

     if (cb->size != 0) freeBuffer(cb);
     
     pthread_mutex_unlock(&cb->mutex);
     return;
}


float readBuffer(CircularBuffer * cb, int position)
{
     pthread_mutex_lock(&cb->mutex);

     if (cb->size == 0) return -1; // Buffer is empty

     float value = cb->data[position];
     
     pthread_mutex_unlock(&cb->mutex);
     return value;
}

void processBuffer(CircularBuffer * cb)
{
     printf("\e[1;1H\e[2J");
     //if (cb->count < BUFFER_SIZE - 1) return;
     
     float last = cb->data[cb->size - 1];
     float first = cb->data[0];
     
     float change = 2 * (last - first) / (last + first);
     printf("%f %f %+10.3f\n", first, last, change * 100);
}

void printBuffer(const CircularBuffer * cb)
{
     printf("\e[1;1H\e[2J");
     printf("Buffer: ");
     for (size_t i = 0; i < cb->size; i++)
     {
          int index = (int)i;// (cb->head + i) % cb->size;
          float fata = cb->data[index];
          printf("%5.1f ", fata);
     }
     printf("\n");
}



void freeBuffer(CircularBuffer *cb)
{
     if (cb)
     {
          free(cb->data);
          cb->data = NULL;
          cb->count = 0;
          cb->size = 0;
     }
}



float getCPUTemperature(const char * token)
{
     FILE *fp;
     char path[MAX_LINE_LENGTH];
     float temperature = 0.0;
     int found = 0;

     fp = popen(TEMP_SRC, "r");
     if (fp == NULL)
     {
          printf("popen failed\n");
          perror("popen failed");
          return -1;
     }

     // Read the output line by line
     while (fgets(path, MAX_LINE_LENGTH, fp) != NULL)
     {
          // Check if the line contains "Core 0"
          if (strstr(path, token) != NULL)
          {
               // Extract the temperature value
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
     } else {
          ;//printf("getCPUTemperature: %f\n", temperature);
     }
     return temperature;
}

void * temperatureProducer(void * arg)
{
     CircularBuffer * cb = (CircularBuffer *)arg;
     while (1)
     {
          float temp = getCPUTemperature(cb->token);
          //printf("temperatureProducer: %f\n", temp);
          if (temp > 0) addFloatToBuffer(cb, temp);
          sleep(UPDATE_TIME);
     }
     return NULL;
}

