#ifndef K3SOLO_H
#define K3SOLO_H
#include <iostream>
#include <vector>

class K3Solo {
private:
     std::vector<bool> bector;
     bool aol(size_t);
     void info(float, const char*) const;

public:
     K3Solo(size_t size) : bector(size, false) {}
     void setall(bool);
     void setone(size_t, bool);
     void flip(size_t);

     bool status(size_t);
     bool* is(size_t);
     void print() const;
};

#endif
