#include "K3Proc.h"

K3Proc::K3Proc()
{
     this->size = 0;
}

void K3Proc::info(float value, const char* description = "")
{
     fprintf(stdout, "K3Proc %f %s", value, description);
}

void K3Proc::append(const char* arg, ...)
{
     va_list arglist;
     const char* nextarg;

     va_start(arglist, arg);
     this->appends(arg);
     while ((nextarg = va_arg(arglist, const char*)) != nullptr) this->appends(nextarg);
     va_end(arglist);
}

void K3Proc::appends(const char* name)
{
     //Freedom* free = new Freedom;
     //*(free->valeur) = 0;
     //*(free->text) = "";

     
     this->buffer[name] = new Freedom();     
     this->size ++;
     this->info(1, name);
}

void K3Proc::remove(const char* name)
{
     auto it = this->buffer.find(name);
     if (it != this->buffer.end()) {
          delete it->second;
          this->buffer.erase(it);
     }
     this->info(0, name);
}

Freedom* K3Proc::get(const char* name)
{
     auto it = this->buffer.find(name);
     if (it != this->buffer.end()) return it->second;

     info(0, name);
     info(0, " created\n");

     this->appends(name);
     return this->get(name);
}

void K3Proc::fill(const char* name, float value)
{
     this->get(name)->valeur.push_back(value);
}


const char* K3Proc::setext(const char* name, const char* value)
{
     this->get(name)->text = value;
     return value;
}

void K3Proc::char2fector(const char* name)
{
     Freedom* free = this->get(name);
     this->reset(free);
     //std::vector<float> fector = free->valeur;
     const char* text = free->text;


     std::istringstream iss(text);
     float current;
     for (int i = 0; i < 7; ++i)
     {
          iss >> current;
          free->valeur.push_back(current);
     }

     return;

     
     char* copy = new char(*text);

     // Tokenize the input string based on whitespace
     char* token = strtok(const_cast<char*>(copy), " ");
     while (token != nullptr)
     {
          // Convert token to float and add to the result vector
          float value = std::strtod(token, nullptr);
          free->valeur.push_back(value);

          // Move to the next token
          token = strtok(nullptr, " ");
     }

     //fector->push_back(7);
     delete copy;
}


unsigned int K3Proc::connect()
{
     this->connect("cpufreq", "/proc/cpuinfo", "cpu MHz");
     this->get_sysinfo("totalmem", "freemem", "uptime", "procs");
     this->get_statvfs("totalspace", "freespace");
     this->processor("cpunumber");
     this->connect("procstat", "/proc/stat");
     this->connect("procloadavg", "/proc/loadavg");
     return 0;
}


void K3Proc::connect(const char* name, const char* path)
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

void K3Proc::file2char(const char* name, const char* path)
{
     FILE* file = fopen(path, "r");
     if (file == NULL) return;

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


void K3Proc::connect(const char* name, const char* path, const char* directive)
{
     FILE* file = fopen(path, "r");
     if (file == NULL) return;
     char line[256];

     //const char* line = this->connect(name, path);

     float valeur = -1.0;

     while (fgets(line, sizeof(line), file) != NULL)
     {
          if (strstr(line, directive))
          {
               char* token = strtok(line, ":");
               if (token != NULL)
               {
                    token = strtok(NULL, ":");
                    if (token != NULL)
                    {
                         valeur = atof(token);
                         break;
                    }
               }
          }
     }

     fclose(file);

     this->get(name)->valeur.push_back(valeur);
}

void K3Proc::processor(const char* cpu_number)
{
     unsigned int cpu_num;
     getcpu(&cpu_num, nullptr);
     this->get(cpu_number)->valeur.push_back(cpu_num);

     //long freq = sysconf(_SC_CLK_TCK);

     //this->get(cpu_number)->valeur.push_back(static_cast<double>(sysconf(_SC_CLK_TCK)));
//     int cpu_count = cpufreq_get_num_cpus();

     //int number_of_processors = get_nprocs();
     //info(number_of_processors, "wow\n");

}

void K3Proc::get_sysinfo(const char* total, const char* free, const char* uptime, const char* procs)
{
     if (sysinfo(&this->struct_sysinfo) != 0) return;
     this->get(total)->valeur.push_back(this->struct_sysinfo.totalram * this->struct_sysinfo.mem_unit);
     this->get(free)->valeur.push_back(this->struct_sysinfo.freeram * this->struct_sysinfo.mem_unit);
     this->get(uptime)->valeur.push_back(this->struct_sysinfo.uptime);
     this->get(procs)->valeur.push_back(this->struct_sysinfo.procs);


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

void K3Proc::get_statvfs(const char* total, const char* free)
{
     if (statvfs("/", &this->struct_statvfs) != 0) return;
     this->get(total)->valeur.push_back(this->struct_statvfs.f_blocks * this->struct_statvfs.f_frsize);
     this->get(free)->valeur.push_back(this->struct_statvfs.f_bfree * this->struct_statvfs.f_frsize);
}


void K3Proc::reset(const char* name)
{
     Freedom* yeah = this->get(name);
     this->reset(yeah);
}

void K3Proc::reset(Freedom* yeah)
{
     if (yeah == nullptr) return;     
     yeah->valeur.clear();
     //fector->text = 0;
}

void K3Proc::dump(Freedom* yeah)
{
     this->info(yeah->valeur.back());
}

void K3Proc::dump()
{
     for (auto& pair : this->buffer)
     {
          this->info(3, pair.first);
          this->dump(pair.second);
     }
}

void K3Proc::reset()
{
     for (auto& pair : this->buffer) this->reset(pair.first);
}

K3Proc::~K3Proc()
{
     for (auto& pair : this->buffer)
     {
          this->info(0, pair.first);
          info(0, pair.first);
          delete pair.second;
     }
}
