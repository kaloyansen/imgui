#include "K3Key.h"

void K3Key::info(float x, const char* err = "") const
{
     fprintf(stdout, "K3Key %f %s", x, err);
}

bool* K3Key::is(size_t index)
{
     if (this->out_of_range(index)) return nullptr;

     std::vector<bool>::iterator it = bector.begin() + index;
     void* baddress = &it;
     return (bool*)baddress;
}

bool K3Key::out_of_range(size_t index) const
{
     if (index < this->bector.size()) return false;

     info(index, "out of control");
     return true;
}

bool K3Key::status(size_t index) const
{
     if (this->out_of_range(index)) return false;
     return this->bector[index];
}

void K3Key::flip(size_t index)
{
     if (this->out_of_range(index)) return;
     this->show(index, !this->bector[index]);
}

void K3Key::hide()
{
     std::fill(this->bector.begin(), this->bector.end(), false);
}

void K3Key::show(size_t index, bool status)
{
     if (this->out_of_range(index)) return;
     this->hide();
     this->bector[index] = status;
}

void K3Key::print() const
{
     for (std::vector<bool>::size_type i = 0; i < this->bector.size(); ++i)
          this->info(i, this->bector[i] ? "true" : "false");
}

