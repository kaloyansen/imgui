#include "K3Proc.h"

K3Proc::K3Proc()
{
     this->size = 0;
}

void K3Proc::info(float value, const char* description = "")
{
     fprintf(stdout, "K3Proc %f %s", value, description);
}

void K3Proc::append(const char* arg, ...)
{
     va_list arglist;
     const char* nextarg;

     va_start(arglist, arg);
     this->appends(arg);
     while ((nextarg = va_arg(arglist, const char*)) != nullptr) this->appends(nextarg);
     va_end(arglist);
}

void K3Proc::appends(const char* name)
{
     this->buffer[name] = new freedom;
     this->size++;
     this->info(1, name);
}

void K3Proc::remove(const char* name)
{
     auto it = this->buffer.find(name);
     if (it != this->buffer.end()) {
          delete it->second;
          this->buffer.erase(it);
     }
     this->info(0, name);
}

freedom* K3Proc::get(const char* name)
{
     auto it = this->buffer.find(name);
     if (it != this->buffer.end()) return it->second;
     else return nullptr;
}

void K3Proc::fill(const char* name, float value)
{
     float* fector = this->get(name)->value;
     if (fector == nullptr) return;
     *fector = value;
}

void K3Proc::connect(const char* name, const char* path, const char* directive)
{
     FILE* file = fopen(path, "r");
     if (file == NULL) return;

     char line[256];
     double valeur = -1.0;

     while (fgets(line, sizeof(line), file) != NULL)
     {
          if (strstr(line, directive))
          {
               char* token = strtok(line, ":");
               if (token != NULL)
               {
                    token = strtok(NULL, ":");
                    if (token != NULL)
                    {
                         valeur = atof(token);
                         break;
                    }
               }
          }
     }

     fclose(file);

     float* data = this->get(name)->value;
     *data = valeur;
}

void K3Proc::connect(const char* name, const char* path)
{
     FILE* file = fopen(path, "r");
     if (file == NULL) return;

     char line[256];
     fgets(line, sizeof(line), file);

     fclose(file);

     std::string* data = this->get(name)->text;

     *data = line;
}

void K3Proc::reset(const char* name)
{
     freedom* fector = this->get(name);
     if (fector == nullptr) return;
     *fector->value = 0;
     *fector->text = "";
}

void K3Proc::dump(float* fector)
{
     this->info(*fector);
}

void K3Proc::dump()
{
     for (auto& pair : this->buffer)
     {
          this->info(3, pair.first);
          this->dump(pair.second->value);
     }
}

void K3Proc::reset()
{
     for (auto& pair : this->buffer) this->reset(pair.first);
}

K3Proc::~K3Proc()
{
     for (auto& pair : this->buffer)
     {
          this->info(0, pair.first);
          delete pair.second;
     }
}
