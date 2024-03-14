#ifndef K3BUFFER_H
#define K3BUFFER_H
#define CODE_BY "Kaloyan Krastev"
#include <stdio.h>
#include <vector>
#include <cmath>
#include <algorithm>

/**
 * @brief a linked list of data containers
 *
 * not breaf
 */
struct K3List
{
     const char* name; /*!< container label */
     std::vector<float>* buffer; /*!< container data */
     float mini; /*!< data absolute minimum */
     float maxi; /*!< data absolute maximum */
     struct K3List* next; /*!< a reference to the next container */
};

/**
 * @brief class to control data containers
 *
 * not breaf   
 */
class K3Buffer
{
private:
     struct K3List* head; /*!< a reference to the first data container */
     int buffer_size; /*!< the buffer size limit */

     std::vector<float>* get(const char*); /*!< a buffer getter */
     void info(float, const char*); /*!< command-line output */
     struct K3List* emerge(const char*); /*!< init container */
     void reset(std::vector<float>*); /*!< reset data */
     void dump(std::vector<float>*); /*!< dump data to stdout */
     void setminmax(struct K3List*, float); /*!< set absolute values */
     void statistique(std::vector<float>, int, float, float, float*, float*); /*!< calculate histogram mean and sigma */

public:
     ~K3Buffer(); /*!< a public destructor */
/**
 * @brief public constructor
 *
 * not breaf   
 */
     K3Buffer(int size) : head(nullptr), buffer_size(size) {}

     struct K3List* node(const char*); /*!< a container getter */
     const char* overtext(const char*, float, float, float, const char*, float, float); /*!< formatted text overlay */

     float min(std::vector<float>*);  /*!< calculate data min */
     float max(std::vector<float>*);  /*!< calculate data max */

     void reset(); /*!< reset containers */
     void dump(); /*!< dump real-time data to stdout */
     void fill(const char*, float); /*!< update data */
     void build(const char*, std::vector<float>*, float*, float*, float*, float*, float*, float*, float*); /*!< create histogram */     
};

#endif // K3BUFFER_H


