#ifndef K3SYSTEM_H
#define K3SYSTEM_H
#define CODE_BY "Kaloyan Krastev"
#include <fstream>
#include <sstream>
#include <vector>
//#include <cstdarg>
#include <cstring>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>

/**
 * @brief a linked list of feature containers
 *
 * not breaf
 */
struct K3Free
{
     const char* name; /*!< the feature label */
     const char* text; /*!< text identifier */
     std::vector<float>* valeur; /*!< float data */
     struct K3Free* next; /*!< a reference to the next element */
};

/**
 * @brief a collection of methods including system calls and functions to access hardware performance information with no need to parse files to increase the efficiency and provide better control over the data retrieval process
 *
 * not breaf
 */
class K3System
{
private:
     struct K3Free* head; /*!< a reference to data containers */
     int size;

     struct sysinfo struct_sysinfo;
     struct statvfs struct_statvfs;
     // struct cpufreq_stats* struct_cpufreq_stats;

     struct K3Free* emerge(const char*);
     std::vector<float>* content(const char*);  /*!< data container */

     std::vector<std::string> split(const std::string&, char);
     double getCPUUsage();

     void reset(const char*);
     void reset(struct K3Free*);
     void file2char(const char*, const char*);
     const char* setext(const char*, const char*);
     void char2fector(const char*);
     void info(float, const char*);

public:
     ~K3System(); /*!< public destructor */
     K3System() : head(nullptr), size(0) {} /*!< public constructor */

     struct K3Free* node(const char*);  /*!< data container getter */
     float back(const char*); /*!< last data value */
     float back(const char*, int); /*!< reference to data */
     unsigned int connect(); /*!< probe system */

     void reset();
     void fill(const char*, float);
     void connect(const char*, const char*);
     void connect(const char*, const char*, const char*);
     void processor(const char*);
     void get_sysinfo(const char*, const char*, const char*, const char *);
     void get_statvfs(const char*, const char*);
     const char* author(void);
};

#endif


