#include "K3System.h"

K3System::~K3System()
{
     struct Freedom* f = this->head;
     while (f != nullptr)
     {
          this->info(0, f->name);
          delete f->valeur;
          this->info(0, "deleted\n");
          f = f->next;
     }
}

void K3System::info(float value, const char* description = "")
{
     printf("K3System %f %s", value, description);
}

struct Freedom* K3System::emerge(const char* name)
{
     struct Freedom* f = this->head;
     struct Freedom* freedom = new Freedom;
     freedom->name = name;
     freedom->valeur = new std::vector<float>;
     freedom->next = nullptr;

     if (f != nullptr)
     {
          while (f->next != nullptr) f = f->next;
          this->info(this->size, name);
          f->next = freedom;
     }
     else
     {
          this->info(1, "object created\n");
          this->head = freedom;
     }

     this->size ++;
     this->info(this->size, name);
     this->info(1, "created\n");
     return freedom;
}

struct Freedom* K3System::node(const char* name)
{
     struct Freedom* f = this->head;
     while (f != nullptr)
     {
          if (f->name == name) return f;
          f = f->next;
     }

     return this->emerge(name);
}

float K3System::back(const char* name, int n)
{
     std::vector<float>* fector = this->content(name);
     float* element = &((*fector)[n]);
     return *element;
     //return &((*fector)[n]);
}

float K3System::back(const char* name)
{
     std::vector<float>* fector = this->content(name);
     return fector->back();
}

std::vector<float>* K3System::content(const char* name)
{
     struct Freedom* f = this->node(name);
     return f->valeur;
}

void K3System::fill(const char* name, float value)
{
     this->content(name)->push_back(value);
}


const char* K3System::author(void)
{
     return CODE_BY;
}

const char* K3System::setext(const char* name, const char* value)
{
     this->node(name)->text = value;
     return value;
}

void K3System::char2fector(const char* name)
{
     struct Freedom* f = this->node(name);
     //this->reset(f);
     //std::vector<float> fector = f->valeur;
     const char* text = f->text;


     std::istringstream iss(text);
     float current;
     for (int i = 0; i < 7; ++i)
     {
          iss >> current;
          f->valeur->push_back(current);
     }

     return;

     char* copy = new char(*text);

     // Tokenize the input string based on whitespace
     char* token = strtok(const_cast<char*>(copy), " ");
     while (token != nullptr)
     {
          // Convert token to float and add to the result vector
          float value = std::strtod(token, nullptr);
          f->valeur->push_back(value);

          // Move to the next token
          token = strtok(nullptr, " ");
     }

     //fector->push_back(7);
     delete copy;
}


unsigned int K3System::connect()
{
     this->connect("cpufreq", "/proc/cpuinfo", "cpu MHz");
     this->get_sysinfo("totalmem", "freemem", "uptime", "procs");
     this->get_statvfs("totalspace", "freespace");
     this->processor("cpunumber");
     this->connect("procstat", "/proc/stat");
     this->connect("procloadavg", "/proc/loadavg");
     return 0;
}


void K3System::connect(const char* name, const char* path)
{
     //const char* str =
     this->file2char(name, path);
     this->char2fector(name);
     //char* message = new char;
     //sprintf(message, "%s", str);
     //info(7, message);
     //delete message;
     //this->setext(name, str);
     //delete str;
}

void K3System::file2char(const char* name, const char* path)
{
     FILE* file = fopen(path, "r");
     if (file == nullptr) return;

     char* line = new char[128];
     fgets(line, 128, file);

     //info(sizeof(line), line);
     fclose(file);

     this->setext(name, line);
     //delete line;
     return;



//      std::ifstream file(path);//, std::ifstream::binary);
//      if (!file.is_open()) {
//           this->info(3, "\ncannot open file");
//           return;
//      }

//      //file.seekg(0, std::ios::end);
//      //std::streampos filesize = file.tellg();

//      char jline[256];
//      fgets(jline, sizeof(jline), file);
//      std::string line;
//      std::streampos filesize = 0; // Initialize the file size

//      std::getline(file, line);
// //     std::getline(file, line);
// //     std::getline(file, line);
//      filesize = line.length();
//      // while (std::getline(file, line))
//      // {
//      //      filesize += line.length() + 1;
//      //      info(line.length(), line.c_str());
//      // }

//      //file.seekg (0, file.end);
//      //int filesize = file.tellg();
//      //file.seekg (0, file.beg);

//      if (filesize <= 0) this->info(filesize, "file size\n");

//      //this->info(7, line.c_str());
//      const char* linec = line.c_str();
//      //const char* well = new char(*linec);
//      //this->info(4, linec);
//      //this->info(9, well);


//      this->setext(name, linec);
//      //return linec;//line.c_str();

//      char* coutput = new char[static_cast<size_t>(filesize)]; // +1 for null terminator

//      if (!coutput) {
//           this->info(3, "\nmemory allocation failed");
//           return;
//      }

//      // if (
//      file.read(coutput, filesize);
//      //      )
//      // {
//      //      info(3, "cannot read file\n");
//      //      return;
//      // }

//      //coutput[filesize] = '\0'; // Null-terminate the string

//      file.close();
//      this->setext(name, coutput);
//      //info(filesize, coutput);
//      //delete coutput;
//      //return coutput;
//      //return output.data();
}


void K3System::connect(const char* name, const char* path, const char* directive)
{
     FILE* file = fopen(path, "r");
     if (file == nullptr) return;
     char line[256];

     //const char* line = this->connect(name, path);

     float valeur = -1.0;

     while (fgets(line, sizeof(line), file) != NULL)
     {
          if (strstr(line, directive))
          {
               char* token = strtok(line, ":");
               if (token != nullptr)
               {
                    token = strtok(NULL, ":");
                    if (token != nullptr)
                    {
                         valeur = atof(token);
                         break;
                    }
               }
          }
     }

     fclose(file);

     this->content(name)->push_back(valeur);
}

void K3System::processor(const char* cpu_number)
{
     unsigned int cpu_num;
     getcpu(&cpu_num, nullptr);
     this->content(cpu_number)->push_back(cpu_num);

//unsigned long cpufreq = cpufreq_get(0);
     //cpufreq_init();
     //cpufreq_policy* policy = cpufreq_get_policy(0);

//     if (policy != nullptr) {

     ;
//cpufreq_get_freq(policy);
     //info(policy->cur, "= CPU 0 frequency");
     //cpufreq_put_policy(policy);
//     } else {
//          info(7, "Error: Unable to get CPU frequency policy");
//     }




     //long freq = sysconf(_SC_CLK_TCK);

     //this->get(cpu_number)->valeur.push_back(static_cast<double>(sysconf(_SC_CLK_TCK)));
//     int cpu_count = cpufreq_get_num_cpus();

     //int number_of_processors = get_nprocs();
     //info(number_of_processors, "wow\n");

}

void K3System::get_sysinfo(const char* total, const char* free, const char* uptime, const char* procs)
{
     if (sysinfo(&this->struct_sysinfo) != 0) return;
     this->content(total)->push_back(this->struct_sysinfo.totalram * this->struct_sysinfo.mem_unit);
     this->content(free)->push_back(this->struct_sysinfo.freeram * this->struct_sysinfo.mem_unit);
     this->content(uptime)->push_back(this->struct_sysinfo.uptime);
     this->content(procs)->push_back(this->struct_sysinfo.procs);


     // long double total_time = struct_sysinfo.totalram - struct_sysinfo.freeram;
     // total_time += struct_sysinfo.totalswap - struct_sysinfo.freeswap;
     // total_time += struct_sysinfo.sharedram;
     // total_time += struct_sysinfo.bufferram;

     // long double total_idle_time = struct_sysinfo.idle;
     // total_idle_time += struct_sysinfo.bufferram;
     // total_idle_time += struct_sysinfo.freeram;
     // total_idle_time += struct_sysinfo.freeswap;

     // long double total_usage = total_time - total_idle_time;
     // double processor_usage = static_cast<double>(total_usage / total_time) * 100.0;

}

void K3System::get_statvfs(const char* total, const char* free)
{
     if (statvfs("/", &this->struct_statvfs) != 0) return;
     this->content(total)->push_back(this->struct_statvfs.f_blocks * this->struct_statvfs.f_frsize);
     this->content(free)->push_back(this->struct_statvfs.f_bfree * this->struct_statvfs.f_frsize);
}


void K3System::reset(const char* name)
{
     struct Freedom* yeah = this->node(name);
     this->reset(yeah);
}

void K3System::reset(struct Freedom* yeah)
{
     if (yeah == nullptr) return;
     yeah->valeur->clear();
     //fector->text = 0;
}

void K3System::reset()
{
     struct Freedom* node = this->head;
     while (node != nullptr)
     {
          this->reset(node);
          node = node->next;
     }

     //for (auto& pair : this->cage) this->reset(pair.first);
}


std::vector<std::string> K3System::split(const std::string& s, char delimiter)
{
     std::vector<std::string> tokens;
     std::string token;
     std::istringstream tokenStream(s);
     while (std::getline(tokenStream, token, delimiter))
          if (!token.empty())
               tokens.push_back(token);

     return tokens;
}


double K3System::getCPUUsage() {

     std::ifstream statFile("/proc/stat");
     if (!statFile.is_open()) return -1.0; // Failed to open /proc/stat

     std::string line;
     std::getline(statFile, line);
     statFile.close();

     // Split the line into tokens
     std::vector<std::string> token = this->split(line, ' ');

     // Calculate total CPU time
     long long totalCPUTime = 0;
     long long value;

     for (size_t i = 1; i < token.size(); ++i)
     {
          try
          {
               value = std::stoll(token[i]);
          }
          catch (const std::exception& e)
          {
               info(1, "error converting token to long long: ");
          }
     }

     totalCPUTime += value;

     // Calculate idle CPU time
     long long idleCPUTime = std::stoll(token[4]);

     // Calculate CPU usage
     double cpuUsage = 100.0 * (1.0 - static_cast<double>(idleCPUTime) / totalCPUTime);

     return cpuUsage;
}

/*
  unsigned int K3System::get_cpufreq_stats(unsigned int processor_number)
  {
  //unsigned int ncpu = cpufreq_get_num_cpus();
  //return ncpu;

//cpufreq_init();
//cpufreq_put_stats(this->struct_cpufreq_stats);
//if (this->struct_cpufreq_stats == nullptr) return 1;
long long unsigned total_time;
struct cpufreq_stats* struct_new = cpufreq_get_stats(processor_number, &total_time);
if (!struct_new) return int(total_time);
unsigned long frequence = struct_new->frequency;
//cpufreq_exit();
return int(frequence);
}
*/
