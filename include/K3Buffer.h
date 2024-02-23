#ifndef K3BUFFER_H
#define K3BUFFER_H
#include <iostream>
#include <vector>
#include <map>
#include <cstdarg>
#include <cmath>
#include <algorithm>

struct ensamble
{
     std::vector<float>* buffer; /*!< data */
     float mini; /*!< absolute minimum */
     float maxi; /*!< absolute maximum */
};

/*! this is a class to contro data buffers */
class K3Buffer
{
private:
     size_t buffer_size_max; /*!< buffer size limit */
     std::map<const char*, ensamble*> fish;

     void appends(const char*);
     void reset(std::vector<float>*);
     void dump(std::vector<float>*);
     void setminmax(ensamble*, float);
     void statistique(std::vector<float>, int, float, float,
                      float*, float*);

public:
     K3Buffer(size_t);
     ~K3Buffer();

     ensamble* fisher(const char*);
     void info(float, const char*);
     std::vector<float>* get(const char*);
     float min(std::vector<float>*);
     float max(std::vector<float>*);

     void reset();
     void dump();
     void append(const char*, ...);
     void remove(const char*);
     void fill(const char*, float);
     void process(std::vector<float>*, float*, float*);
     void calcule(const char*, std::vector<float>*, float*, float*, float*, float*, float*, float*, float*);
     const char* overtext(const char*, float, float, float, const char*, float, float);
};

#endif


