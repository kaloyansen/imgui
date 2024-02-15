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
     else return nullptr;
}

void K3Buffer::fill(const char*name, float value)
{
     std::vector<float>* fector = this->get(name);
     fector->push_back(value);
     if (fector->size() > this->buffer_size_max) fector->erase(fector->begin());
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
