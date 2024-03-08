#include "K3Key.h"

void K3Key::info(float x, const char* err = "") const
{
     printf("K3Key %f %s", x, err);
}

bool* K3Key::is(int index)
{
     if (this->out_of_range(index)) return nullptr;

     void* address = &bector + index;
     return (bool*)address;
}

bool K3Key::out_of_range(int index) const
{
     if (index < this->bize) return false;

     info(index, "out of range");
     return true;
}

bool K3Key::status(int index) const
{
     if (this->out_of_range(index)) return false;
     return this->bector[index];
}

void K3Key::flip(int index)
{
     if (this->out_of_range(index)) return;
     this->show(index, !this->bector[index]);
}

void K3Key::hide(void)
{
     std::fill(this->bector.begin(), this->bector.end(), false);
}

void K3Key::show(int index, bool status)
{
     if (this->out_of_range(index)) return;
     this->hide();
     this->bector[index] = status;
}

