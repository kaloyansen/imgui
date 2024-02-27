#ifndef K3KEY_H
#define K3KEY_H
#include <stdio.h>
#include <vector>

class K3Key {
private:
     int bize;
     std::vector<bool> bector;
     bool out_of_range(int) const;
     void info(float, const char*) const;

public:
     K3Key(int size) : bector(size, false) { this->bize = size; }
     bool* is(int);
     bool status(int) const;

     void hide(void);
     void show(int, bool);
     void flip(int);

     void print(void) const;
};

#endif
