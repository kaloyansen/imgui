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

/*! this is a class to control data buffers */
class K3Buffer
{
private:
     size_t buffer_size_max; /*!< buffer size limit */
     std::map<const char*, ensamble*> fish;

     void info(float, const char*); /*!< command-line output */
     void appends(const char*); /*!< create a new buffer */
     void reset(std::vector<float>*); /*!< reset a buffer */
     void dump(std::vector<float>*);
     void setminmax(ensamble*, float); /*!< an absolute setter */
     void statistique(std::vector<float>, int, float, float,
                      float*, float*); /*!< calculate histogram mean and sigma */

public:
     K3Buffer(size_t);
     ~K3Buffer();

     ensamble* fisher(const char*); /*!< ensamble getter */
     std::vector<float>* get(const char*); /*!< buffer getter */
     float min(std::vector<float>*);  /*!< calculate buffer minimum */
     float max(std::vector<float>*);  /*!< calculate buffer maximum */

     void reset(); /*!< reset all buffers */
     void dump();
     void append(const char*, ...); /*!< deprecated method */
     void remove(const char*); /*!< deprecated method */
     void fill(const char*, float); /*!< update buffer */
     void build(const char*, std::vector<float>*, float*, float*, float*, float*, float*, float*, float*); /*!< create histogram */
     const char* overtext(const char*, float, float, float, const char*, float, float); /*!< formatted text overlay */
};

#endif


