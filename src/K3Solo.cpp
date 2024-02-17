#include "K3Solo.h"

void K3Solo::info(float x, const char* err = "") const
{
     fprintf(stdout, "K3Solo %f %s", x, err);
}


bool K3Solo::aol(size_t index)
{
     if (index < this->bector.size()) return false;

     info(index, "out of control");
     return true;
}

bool* K3Solo::is(size_t index)
{
     if (this->aol(index)) return nullptr;

     std::vector<bool>::iterator it = bector.begin() + index;
     void* baddress = &it;
     return (bool*)baddress;
}


bool K3Solo::status(size_t index)
{
     if (this->aol(index)) return false;
     return this->bector[index];
}

void K3Solo::flip(size_t index)
{
     if (this->aol(index)) return;
     this->setone(index, !this->bector[index]);
}

void K3Solo::setall(bool status)
{
     std::fill(this->bector.begin(), this->bector.end(), status);
}

void K3Solo::setone(size_t index, bool status)
{
     if (this->aol(index)) return;
     this->setall(false);
     this->bector[index] = status;
}

void K3Solo::print() const
{
     for (std::vector<bool>::size_type i = 0; i < this->bector.size(); ++i)
          this->info(i, this->bector[i] ? "true" : "false");
}

