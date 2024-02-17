#ifndef K3PROC_H
#define K3PROC_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <cstdarg>
#include <cstring>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <unistd.h>
/* use system calls and functions to access hardware performance information directly with no need to parse files is more efficient and provides better control over the data retrieval process */
struct Freedom {
     std::vector<float> valeur;
     const char* text;
};

class K3Proc {
private:
     size_t size;
     std::map<const char*, Freedom*> cage;
     struct sysinfo struct_sysinfo;
     struct statvfs struct_statvfs;

     std::vector<std::string> split(const std::string&, char);
     double getCPUUsage();

     void appends(const char*);
     void reset(const char*);
     void reset(Freedom*);
     void dump(Freedom*);
     void file2char(const char*, const char*);
     const char* setext(const char*, const char*);
     void char2fector(const char*);


public:
     K3Proc();
     ~K3Proc();

     Freedom* get(const char*); 
     void info(float, const char*);
     unsigned int connect();

     void reset();
     void dump();
     void append(const char*, ...);
     void remove(const char*);
     void fill(const char*, float);
     void connect(const char*, const char*);
     void connect(const char*, const char*, const char*);
     void processor(const char*);
     void get_sysinfo(const char*, const char*, const char*, const char *);
     void get_statvfs(const char*, const char*);
};

#endif


