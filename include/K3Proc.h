#ifndef K3PROC_H
#define K3PROC_H
#include <iostream>
#include <vector>
#include <map>
#include <cstdarg>
#include <algorithm>
#include <string>
//#include <sstream>
#include <cstring>

struct freedom {
     float* value;
     std::string* text;
};

class K3Proc {
private:
     std::map<const char*, freedom*> buffer;
     size_t size;

     void appends(const char*);
     void reset(const char*);
     void dump(float*);

public:
     K3Proc();
     ~K3Proc();

     freedom* get(const char*);
     void info(float, const char*);

     void reset();
     void dump();
     void append(const char*, ...);
     void remove(const char*);
     void fill(const char*, float);
     void connect(const char*, const char*);
     void connect(const char*, const char*, const char*);
};

#endif


