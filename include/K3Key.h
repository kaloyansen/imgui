#ifndef K3KEY_H
#define K3KEY_H
#include <iostream>
#include <vector>

class K3Key {
private:
     std::vector<bool> bector;
     bool out_of_range(size_t) const;
     void info(float, const char*) const;

public:
     K3Key(size_t size) : bector(size, false) {}
     bool* is(size_t);
     bool status(size_t) const;

     void hide(void);
     void show(size_t, bool);
     void flip(size_t);

     void print() const;
};

#endif
