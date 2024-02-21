#include "K3Buffer.h"

K3Buffer::K3Buffer(size_t size)
{
     this->buffer_size_max = size;
}

void K3Buffer::info(float value, const char* description = "")
{
     fprintf(stdout, "K3Buffer %f %s", value, description);
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
     std::vector<float>* fector = new std::vector<float>;
     this->buffer[name] = fector;
     this->info(1, name);
}

void K3Buffer::remove(const char* name)
{
     auto it = this->buffer.find(name);
     if (it != this->buffer.end()) {
          delete it->second;
          this->buffer.erase(it);
     }
     this->info(0, name);
}


std::vector<float>* K3Buffer::get(const char* name)
{
     auto it = this->buffer.find(name);
     if (it != this->buffer.end()) return it->second;

     info(0, name);
     info(0, " created\n");

     this->appends(name);
     return this->get(name);
}

void K3Buffer::fill(const char* name, float value)
{
     std::vector<float>* fector = this->get(name);
     fector->push_back(value);
     if (fector->size() > this->buffer_size_max) fector->erase(fector->begin());
}

/*
  std::vector<float> K3Buffer::histogram(const std::vector<float>& data, int num_bins)
  {
  float min_val = *std::min_element(data.begin(), data.end());
  float max_val = *std::max_element(data.begin(), data.end());
  float bin_width = (max_val - min_val) / num_bins;
  std::vector<float> histo(num_bins, 0);

  for (float value : data)
  {
  int bin_index = (int)((value - min_val) / bin_width);
  float old_value = histo[bin_index];
  if (bin_index >= 0 && bin_index < num_bins) histo[bin_index] = old_value + 1;
  }

  return histo;
  }
*/

float K3Buffer::min(std::vector<float>* fector)
{
     return *std::min_element(fector->begin(), fector->end());
}

float K3Buffer::max(std::vector<float>* fector)
{
     return *std::max_element(fector->begin(), fector->end());
}

void K3Buffer::calcule(const char* name, const int binum,
                       std::vector<float>* histogram,
                       float* hmin, float* hmax, float* bmin, float* bmax, float* cur)
{
     std::vector<float>* fector = this->get(name);
     *bmin = this->min(fector);//*std::min_element(fector->begin(), fector->end());
     *bmax = this->max(fector);//*std::max_element(fector->begin(), fector->end());
     *cur = fector->back();

     float bin_width = (*bmax - *bmin) / binum;

     for (size_t i = 0; i < fector->size(); i++)
     {
          float value = (*fector)[i];
          int bin_index = (int)((value - *bmin) / bin_width);
          if (bin_index >= 0 && bin_index < binum)
          {
               float old_value = (*histogram)[bin_index];
               (*histogram)[bin_index] = old_value + 1;
          }
     }

     *hmin = this->min(histogram);
     *hmax = this->max(histogram);
}

void K3Buffer::process(std::vector<float>* fector, float* min, float* max)
{
     auto amin = min_element(fector->begin(), fector->end());
     auto amax = max_element(fector->begin(), fector->end());
     *min = float(*amin);
     *max = float(*amax);
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
     for (auto& pair : this->buffer)
     {
          this->info(3, pair.first);
          this->dump(pair.second);
     }
}

void K3Buffer::reset()
{
     for (auto& pair : this->buffer) this->reset(pair.second);
}

K3Buffer::~K3Buffer()
{
     for (auto& pair : this->buffer)
     {
          this->info(0, pair.first);
          delete pair.second;
     }
}
