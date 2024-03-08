#ifndef K3BUFFER_H
#define K3BUFFER_H
#define CODE_BY "Kaloyan Krastev"
#include <stdio.h>
#include <vector>
#include <cmath>
#include <algorithm>

/*! a linked list of data containers corresponding considered operating system features */
struct Feature
{
     const char* name; /*!< the feature label */
     std::vector<float>* buffer; /*!< feature values */
     float mini; /*!< the feature absolute minimum */
     float maxi; /*!< the feature absolute maximum */
     struct Feature* next; /*!< a reference to the next feature */
};

/*! this is a class to control data buffers */
class K3Buffer
{
private:
     struct Feature* head; /*!< a reference to the first data container */
     int buffer_size; /*!< the data buffer size limit */

     std::vector<float>* get(const char*); /*!< a buffer getter */
     void info(float, const char*); /*!< command-line output */
     struct Feature* emerge(const char*); /*!< feature generation */
     void reset(std::vector<float>*); /*!< data initialisation */
     void dump(std::vector<float>*); /*!< dump buffer data to stdout */
     void setminmax(struct Feature*, float); /*!< set absolute values */
     void statistique(std::vector<float>, int, float, float, float*, float*); /*!< calculate histogram mean and sigma */

public:
     ~K3Buffer(); /*!< a public destructor */
     K3Buffer(int size) : head(nullptr), buffer_size(size) {} /*!< a public constructor */

     struct Feature* node(const char*); /*!< a feature getter */
     const char* overtext(const char*, float, float, float, const char*, float, float); /*!< formatted text overlay */

     float min(std::vector<float>*);  /*!< calculate buffer minimum */
     float max(std::vector<float>*);  /*!< calculate buffer maximum */

     void reset(); /*!< reset all buffers */
     void dump(); /*!< dump real-time data to stdout */
     void fill(const char*, float); /*!< update buffer */
     void build(const char*, std::vector<float>*, float*, float*, float*, float*, float*, float*, float*); /*!< create histogram */     
};

#endif


