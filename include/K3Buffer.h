#ifndef K3BUFFER_H
#define K3BUFFER_H
#include <iostream>
#include <vector>
#include <map>
#include <cstdarg>
#include <algorithm>

class K3Buffer {
private:
     size_t buffer_size_max;
     std::map<const char*, std::vector<float>*> buffer;

     void appends(const char*);
     void reset(std::vector<float>*);
     void dump(std::vector<float>*);

public:
     K3Buffer(size_t);
     ~K3Buffer();

     std::vector<float>* get(const char*);
     void info(float, const char*);

     void reset();
     void dump();
     void append(const char*, ...);
     void remove(const char*);
     void fill(const char*, float);
     void process(std::vector<float>*, float*, float*);
};

#endif


