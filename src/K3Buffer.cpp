#include "K3Buffer.h"

K3Buffer::K3Buffer(int size)
{
     this->buffer_size = size;
}

void K3Buffer::info(float value, const char* description = "")
{
     printf("K3Buffer %f %s", value, description);
}

void K3Buffer::append(const char* arg, ...)
{
     va_list arglist;
     const char* nextarg;

     va_start(arglist, arg);
     this->appends(arg);
     while ((nextarg = va_arg(arglist, const char*)) != nullptr) this->appends(nextarg);
     this->info(1, "\n");
     va_end(arglist);
}

void K3Buffer::appends(const char* name)
{
     ensamble* ens = new ensamble;
     ens->buffer = new std::vector<float>;
     ens->mini = 1e10;
     ens->maxi = 0;
     
     this->fish[name] = ens;
     this->info(1, name);
}

void K3Buffer::remove(const char* name)
{
     // auto it = this->buffer.find(name);
     // if (it != this->buffer.end()) {
     //      delete it->second;
     //      this->buffer.erase(it);
     // }
     this->info(0, name);
}


ensamble* K3Buffer::fisher(const char* name)
{
     auto it = this->fish.find(name);
     if (it != this->fish.end()) return it->second;

     this->appends(name);
     return this->fisher(name);
}

void K3Buffer::setminmax(ensamble* ens, float cur)
{
     float minrec = ens->mini;
     float maxrec = ens->maxi;
     ens->maxi = cur > maxrec ? cur : maxrec;
     ens->mini = cur < minrec ? cur : minrec;
}

std::vector<float>* K3Buffer::get(const char* name)
{
     ensamble* ens = this->fisher(name);
     return ens->buffer;
}

void K3Buffer::fill(const char* name, float value)
{
     ensamble* ens = this->fisher(name);
     std::vector<float>* fector = ens->buffer;//this->get(name);
     fector->push_back(value);
     int fize = fector->size();
     if (fize > this->buffer_size) fector->erase(fector->begin());

     this->setminmax(ens, value);
}

float K3Buffer::min(std::vector<float>* fector)
{
     return *std::min_element(fector->begin(), fector->end());
}

float K3Buffer::max(std::vector<float>* fector)
{
     return *std::max_element(fector->begin(), fector->end());
}

void K3Buffer::statistique(std::vector<float> histogram,
                           int N, float bmin, float bmax,
                           float* m, float* d)
{

     // N is the number of measurements with values in the range [bmin; nmax]
     int n = histogram.size(); // the number of histogram bins
     float b = (bmax - bmin) / n; // constant histogram bin width 
     float W = 0.0f; // sum of weights
     float S = 0.0f; // sum of squared differences

     for (int i = 0; i < n; i++)
     {
          int x = histogram[i]; // bin count
          float c = b * (1. / 2 + i) + bmin; // bin center
          float w = x * c; // bin weight
          W += w;
     }

     *m = W / N; // calculated mean

     for (int i = 0; i < n; i++)
     {
          int x = histogram[i];
          float c = b * (1. / 2 + i) + bmin;
          float s = (c - *m) * (c - *m); // the squared difference
          S += x * s;
     }

     float v = S / N; // calculated variation
     *d = std::sqrt(v); // calculated standard deviation     
}

void K3Buffer::build(const char* name, std::vector<float>* histogram,
                     float* hmin, float* hmax, float* hmean, float* hstdev,
                     float* bmin, float* bmax, float* cur)
{
     std::vector<float>* fector = this->get(name);
     *bmin = this->min(fector);
     *bmax = this->max(fector);
     *cur = fector->back();
     float buff_size = fector->size();

     float hist_size = histogram->size();
     float bin_width = (*bmax - *bmin) / hist_size;

     for (int i = 0; i < buff_size; i++)
     {
          float value = (*fector)[i];
          int bin_index = (int)((value - *bmin) / bin_width);
          if (bin_index >= 0 && bin_index < hist_size)
          {
               float old_value = (*histogram)[bin_index];
               (*histogram)[bin_index] = old_value + 1;
          }
     }

     this->statistique(*histogram, buff_size, *bmin, *bmax, hmean, hstdev);
     *hmin = this->min(histogram);
     *hmax = this->max(histogram);
}

void K3Buffer::reset(std::vector<float>* fector)
{
     if (fector != nullptr) fector->clear();
     //fector->assign(fector->size(), 0.0f);
}

void K3Buffer::dump(std::vector<float>* fector)
{
     this->info(fector->back());
}

void K3Buffer::dump()
{
     for (auto& pair : this->fish)
     {
          this->info(3, pair.first);
          this->dump(pair.second->buffer);
     }
}

void K3Buffer::reset()
{
     for (auto& pair : this->fish) this->reset(pair.second->buffer);
}

const char* K3Buffer::overtext(const char* title, float var1, float var2, float var3, const char* sunit, float var4, float var5)
{
     static char ot[100];
     snprintf(ot, sizeof(ot), "%20s %9.2f %9.2f %9.2f %5s %9.2f %9.2f",
              title, var1, var2, var3, sunit, var4, var5);
     return ot;
}


K3Buffer::~K3Buffer()
{
     for (auto& pair : this->fish)
     {
          this->info(0, pair.first);
          delete pair.second->buffer;
     }
}


