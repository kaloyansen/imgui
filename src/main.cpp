#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include "K3Buffer.h"
#include "K3Proc.h"
#include <stdio.h>
#include <stdlib.h>
//#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#ifdef __APPLE__
//#include <sys/sysinfo.h>
#define GL_SILENCE_DEPRECATION
#endif
#include <GLFW/glfw3.h>
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif
#define XVIEW 999
#define YVIEW 666
#define BUFSIZE 600
#define DEBUG true
#define VERSION 0.2

///////////////////////////////////////////////////////////////////////////////////
int screen_width;
int screen_height;
char * sinfo_version = new char[111];

static void glfw_error_callback(int error, const char* description)
{
     fprintf(stderr, "glfw %d %s\n", error, description);
}

std::vector<std::string> split(const std::string& s, char delimiter) {

     std::vector<std::string> tokens;
     std::string token;
     std::istringstream tokenStream(s);
     while (std::getline(tokenStream, token, delimiter))
          if (!token.empty())
               tokens.push_back(token);

     return tokens;
}


double getCPUUsage() {

     std::ifstream statFile("/proc/stat");
     if (!statFile.is_open()) return -1.0; // Failed to open /proc/stat

     std::string line;
     std::getline(statFile, line);
     statFile.close();

     // Split the line into tokens
     std::vector<std::string> token = split(line, ' ');

     // Calculate total CPU time
     long long totalCPUTime = 0;

     for (size_t i = 1; i < token.size(); ++i)
     {
          try
          {
               long long value = std::stoll(token[i]);
               totalCPUTime += value;
          }
          catch (const std::exception& e)
          {
               glfw_error_callback(1, "error converting token to long long: ");
          }
     }

     // Calculate idle CPU time
     long long idleCPUTime = std::stoll(token[4]);

     // Calculate CPU usage
     double cpuUsage = 100.0 * (1.0 - static_cast<double>(idleCPUTime) / totalCPUTime);

     return cpuUsage;
}


void getState(const char* path, std::string* data)
{
     FILE* file = fopen(path, "r");
     if (file == NULL) return;

     char line[256];
     fgets(line, sizeof(line), file);
     *data = line;
     fclose(file);
     

     // std::ifstream file(path);
     // if (!file.is_open()) return;
     // std::getline(file, *data);
}


void updateSystemInfo(long long* totalMem, long long* freeMem,
                      long long* totalSpace, long long* freeSpace,
                      double* loadavgarr, float* cpufreq,
                      std::string* procloadavg, std::string* procstat)
{
//     *cpufreq = getDirectiveValue("/proc/cpuinfo", "cpu MHz");//getCPUUsage();

     struct sysinfo memInfo;
     sysinfo(&memInfo);
     *totalMem = memInfo.totalram * memInfo.mem_unit;
     *freeMem = memInfo.freeram * memInfo.mem_unit;

     struct statvfs vfs;
     if (statvfs("/", &vfs) != 0)
     {// failed to get filesystem statistics
          *totalSpace = 100;
          *freeSpace = 1;

     } else {
          
          *totalSpace = vfs.f_blocks * vfs.f_frsize;
          *freeSpace = vfs.f_bfree * vfs.f_frsize;
     }


     
     // std::ifstream loadFile("/proc/loadavg");
     // if (!loadFile.is_open()) return;
     // std::getline(loadFile, *loadLine);
     getState("/proc/stat", procstat);
     getState("/proc/loadavg", procloadavg);


     std::istringstream iss(*procloadavg);
     for (int i = 0; i < 7; ++i) iss >> loadavgarr[i];

}

void plotBuffer(std::vector<float>* buffer, const char* title = "",
                const char* unit = "", int size = BUFSIZE)
{
     float last = buffer->back();
     char overlay[100];
     float min, max;
     float* pmin = &min;
     float* pmax = &max;

     K3Buffer delme(0);
     delme.process(buffer, pmin, pmax);

     sprintf(overlay, "%s %9.2f %9.2f %9.2f %s", title, min, last, max, unit);

     ImGui::PlotLines("", buffer->data(), size, 0, overlay, min, max, ImVec2(screen_width - 16, screen_height / 10));
}


void bufferMonitor(float buffer[], int size,
                   const char* title = "", const char* unit = "")
{
     float last = buffer[size - 1];
     char overlay[100];

     sprintf(overlay, "%7.1f %s", last, unit);

     ImGui::PlotLines("", buffer, size, size, title);
     ImGui::SameLine();
     ImGui::ProgressBar(last, ImVec2(333.0f, 0.0f), overlay);
}

//
// Main code
int main(int, char**)
{
     glfwSetErrorCallback(glfw_error_callback);
     //glfwSetErrorCallback(error);
     if (!glfwInit())
          return 1;

     glfw_error_callback(-1, "this is not an error\n");

     // Create window with graphics context
     GLFWwindow* window = glfwCreateWindow(XVIEW, YVIEW, "sinfo", nullptr, nullptr);
     if (window == nullptr)
          return 1;
     glfwMakeContextCurrent(window);
     glfwSwapInterval(1); // Enable vsync

     // Setup Dear ImGui context
     IMGUI_CHECKVERSION();
     ImGui::CreateContext();
     ImGuiIO& io = ImGui::GetIO(); (void)io;
     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
     io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

     // Setup Dear ImGui style
     ImGui::StyleColorsClassic();
     //ImGuiStyle& style = ImGui::GetStyle();
     //style.ScaleAllSizes(2);
     //ImGui::StyleColorsLight();

     // Setup Platform/Renderer backends
     ImGui_ImplGlfw_InitForOpenGL(window, true);
     ImGui_ImplOpenGL2_Init();

     bool show_main = true;
     bool show_control = false;
     bool show_about = false;
     bool show_debug = false;
     bool show_demo_window = false;

     bool do_not_update_system_info = false;
     ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

     int procimax = 0;
     int proci;
     

     K3Buffer* K3B = new K3Buffer(BUFSIZE);
     K3Proc* Proc = new K3Proc();     

     int loop = 0;
     int uloop = 0;
     int delay = 1;
     float buftime;
     float font_scale = 1.7f;
     const char* status = "unknown";
     sprintf(sinfo_version, "sinfo v%3.1f", VERSION);

     static ImGuiWindowFlags mainWindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
     static ImGuiWindowFlags controlWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
//     static ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_SizingFixedFit;
     // Main loop
     while (!glfwWindowShouldClose(window))
     {
          loop++;

          glfwPollEvents();

          // Start the Dear ImGui frame
          ImGui_ImplOpenGL2_NewFrame();
          ImGui_ImplGlfw_NewFrame();
          ImGui::NewFrame();

          io.FontGlobalScale = font_scale;


          if (ImGui::IsKeyPressed(ImGuiKey_A))
          {
               show_about = show_about == true ? false : true;
               show_control = false;
               show_debug = false;
          }
          if (ImGui::IsKeyPressed(ImGuiKey_B))
          {
               show_about = false;
               show_control = false;
               show_debug = show_debug == true ? false : true;
          }
          if (ImGui::IsKeyPressed(ImGuiKey_C))
          {
               show_about = false;
               show_control = show_control == true ? false : true;
               show_debug = false;
          }
          

          if (ImGui::IsKeyPressed(ImGuiKey_D)) K3B->dump();
          if (ImGui::IsKeyPressed(ImGuiKey_R))
          {
               K3B->reset();
               uloop = 0;
               delay = 1;
               show_about = false;
               show_debug = false;
               show_control = false;
          }
          if (ImGui::IsKeyPressed(ImGuiKey_Q))
          {
               delete Proc;
               delete K3B;
               glfwSetWindowShouldClose(window, 1);
          }
               
          if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

          delay = delay > 0 ? delay : 1;
          do_not_update_system_info = loop % delay ? true : false;
          if (!do_not_update_system_info)
          {
               uloop++;
               //Proc->connect();
               Proc->connect("cpufreq", "/proc/cpuinfo", "cpu MHz");
               Proc->get_sysinfo("totalmem", "freemem", "uptime", "procs");
               Proc->get_statvfs("totalspace", "freespace");
               Proc->processor("cpunumber");
               Proc->connect("procstat", "/proc/stat");
               Proc->connect("procloadavg", "/proc/loadavg");

               if (uloop < BUFSIZE) status = "load";
               else if (uloop == BUFSIZE) status = "done";
               else status = "run";

               if (strcmp("done", status) == 0) delay = 6;
//               if (status == "done") delay = 6;
               float appfreq = 1.0f / io.DeltaTime;
               float upfreq = appfreq / delay;

               buftime = float(BUFSIZE) / upfreq;
                              
               proci = Proc->get("procloadavg")->valeur[3];
               procimax = proci > procimax ? proci : proci * 2;

//               if (!do_not_update_system_info)
//               {
               K3B->fill("cpunumber", Proc->get("cpunumber")->valeur.back());
               K3B->fill("cpufreq", Proc->get("cpufreq")->valeur.back());
               K3B->fill("freespace", 100 * Proc->get("freespace")->valeur.back() / Proc->get("totalspace")->valeur.back());
               K3B->fill("freemem", 100 * Proc->get("freemem")->valeur.back() / Proc->get("totalmem")->valeur.back());
               K3B->fill("uptime", Proc->get("uptime")->valeur.back());
               K3B->fill("procs", Proc->get("procs")->valeur.back());
               K3B->fill("upfreq", upfreq);
               K3B->fill("appfreq", appfreq);
               K3B->fill("loadavg0", Proc->get("procloadavg")->valeur[0]);
               K3B->fill("loadavg1", Proc->get("procloadavg")->valeur[1]);
               K3B->fill("loadavg2", Proc->get("procloadavg")->valeur[2]);
               K3B->fill("loadavg3", Proc->get("procloadavg")->valeur[3]);
          }

          if (show_main)
          {
               
               const ImGuiViewport* viewport = ImGui::GetMainViewport();
               ImGui::SetNextWindowPos(viewport->WorkPos);
               ImGui::SetNextWindowSize(viewport->WorkSize);
               screen_width = viewport->WorkSize.x;
               screen_height = viewport->WorkSize.y;

               bool  boopen = true;
               bool* poopen = &boopen;

               ImGui::Begin("main", poopen, mainWindowFlags);
//               ImGui::CheckboxFlags("ImGuiWindowFlags_NoTitleBar", &windowFlags, ImGuiWindowFlags_NoTitleBar);
//               ImGui::CheckboxFlags("ImGuiWindowFlags_NoCollapse", &windowFlags, ImGuiWindowFlags_NoCollapse);
//               ImGui::CheckboxFlags("ImGuiWindowFlags_NoScrollbar", &windowFlags, ImGuiWindowFlags_NoScrollbar);



               if (ImGui::SmallButton("[c]ontrol"))
               {
                    show_about = false;
                    show_control = true;
                    show_debug = false;
               }

               ImGui::SameLine();
               ImGui::ProgressBar(0.1f * proci, ImVec2(0, 22), status);

               ImGui::SameLine();
               if (ImGui::SmallButton("[q]uit"))
               {
                    delete Proc;
                    delete K3B;
                    glfwSetWindowShouldClose(window, 1);
               }


/*               if (ImGui::BeginTable("system", 3, tableFlags, ImVec2(screen_width, 0)))
                 {

                 ImGui::TableNextColumn();
                 ImGui::Text("loops/60:%d/%d", loop, loop / 60);

                 //sprintf(buf, "%03d", i);
                 ImGui::TableNextColumn();
                 ImGui::Text("sinfo v%3.1f", VERSION);

                 ImGui::TableNextColumn();
                 ImGui::Text("rate/s:%.3f", 1.0f / io.Framerate);

                 ImGui::TableNextColumn();
                 //ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Pink");
                 ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "FPS:%.1fHz", io.Framerate);


                 ImGui::TableNextColumn();
                 ImGui::Button("[c]ontrol");

                 ImGui::TableNextColumn();
                 ImGui::ProgressBar(0.1f * proci, ImVec2(0.0f, 0.0f), status);

                 ImGui::TableNextColumn();
                 ImGui::Button("[q]uit");

                 ImGui::EndTable();
                 }
*/

               //ImGui::Checkbox("pause", &do_not_update_system_info);
               //ImGui::ColorEdit3("clear color", (float*)&clear_color);

/* buttons return true when clicked like most widgets when edited or activated */
               
               //ImGui::SliderFloat("RAM", &ram, 0.0f, 100.0f);
               //ImGui::SameLine();
               //ImGui::Text("my counter is %d", counter);

               //ImGui::SliderFloat("frequence, Hz", &frequence, 1, 120);
//               sprintf(overlay, "%12.0f Hz", cpufreq * 1000 * 1000);

               // bufferMonitor(bufcpufreq, BUFSIZE, "processor frequence, MHz", 1.0f / 1111);
               // bufferMonitor(bufappfreq, BUFSIZE, "imgui frequence, Hz", 1.0f / 100);
               // bufferMonitor(bufupfreq, BUFSIZE, "update frequence, Hz", 1.0f / 100);
               // bufferMonitor(bufloadavg3, BUFSIZE, "process number", 1.0f / 10);

//               ImGui::Indentssamelin();
//               ImGui::Unindent();

               plotBuffer(K3B->get("loadavg3"), "running processes");
               plotBuffer(K3B->get("procs"), "total processes");
               plotBuffer(K3B->get("cpunumber"), "cpu number");
               plotBuffer(K3B->get("cpufreq"), "processor frequence", "MHz");
               plotBuffer(K3B->get("appfreq"), "imgui frequence", "Hz");
               plotBuffer(K3B->get("upfreq"), "app frequence", "Hz");
               plotBuffer(K3B->get("uptime"), "system uptime", "s");
               plotBuffer(K3B->get("freemem"), "free memory", "%");
               plotBuffer(K3B->get("freespace"), "free storage", "%");

               ImGui::End();
          }

          if (show_debug)
          {
               if (ImGui::Begin("debug", &show_debug, controlWindowFlags))
               {
                    ImGui::SeparatorText("debug");
                    // ImGui::Text("loadavg: %s", procloadavg.c_str());
                    // ImGui::Text("stat: %s", procstat.c_str());
                    ImGui::Text("loadavg: %s", Proc->get("procloadavg")->text);
                    ImGui::Text("stat: %s", Proc->get("procstat")->text);
                    ImGui::End();
               }
          }

          if (show_control)
          {
               if (ImGui::Begin("control", &show_control, controlWindowFlags))
               {
                    if (ImGui::Button("[a]bout"))
                    {
                         show_about = true;
                         show_debug = false;
                         show_control = false;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("de[b]ug"))
                    {
                         show_about = false;
                         show_debug = true;
                         show_control = false;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("[c]lose"))
                    {
                         show_about = false;
                         show_debug = false;
                         show_control = false;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("[d]ump")) K3B->dump();
                    ImGui::SameLine();
                    if (ImGui::Button("[r]eset"))
                    {
                         K3B->reset();
                         uloop = 0;
                         delay = 1;
                         show_about = false;
                         show_debug = false;
                         show_control = false;
                    }

                    ImGui::SeparatorText("");
                    
                    char bufoverlay[33];
                    sprintf(bufoverlay, "history %10.0f seconds", buftime);
                    ImGui::ProgressBar(buftime / 1111, ImVec2(0.0f, 0.0f), bufoverlay);
                    ImGui::SeparatorText("control");
                    ImGui::SliderInt("update time", &delay, 1, 100);
                    ImGui::Separator();
                    ImGui::Separator();
                    ImGui::SliderFloat("font scale", &font_scale, 0.5, 5);
                    ImGui::Separator();
                    ImGui::ShowStyleSelector("color style");
                    ImGui::Separator();
                    ImGui::End();
               }
          }


          if (show_about)
          {
               //show_control = false;
               if (ImGui::Begin("about", &show_about, controlWindowFlags))
               {
                    ImGui::Text("sinfo v%3.1f", VERSION);
                    ImGui::SeparatorText("code");
                    ImGui::Text("Kaloyan Krastev");
                    ImGui::SeparatorText("powered by");
                    ImGui::Text("ImGui");
                    ImGui::SameLine();
                    ImGui::Text("GLFW");
                    ImGui::SameLine();
                    ImGui::Text("OpenGL");
                    ImGui::SameLine();
                    ImGui::Text("X11");
                    ImGui::SeparatorText("copyleft 2023-2024");
                    ImGui::Text("triplehelix-consulting.com");
                    ImGui::Separator();
                    if (ImGui::Button("c[a]ncel")) show_about = false;
                    ImGui::End();
               }
          }


// Rendering
          ImGui::Render();
          int display_w, display_h;
          glfwGetFramebufferSize(window, &display_w, &display_h);
          glViewport(0, 0, display_w, display_h);
          glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
          glClear(GL_COLOR_BUFFER_BIT);

          // If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
          // you may need to backup/reset/restore other state, e.g. for current shader using the commented lines below.
          //GLint last_program;
          //glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
          //glUseProgram(0);
          ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
          //glUseProgram(last_program);

          glfwMakeContextCurrent(window);
          glfwSwapBuffers(window);
     }

     // Cleanup

     ImGui_ImplOpenGL2_Shutdown();
     ImGui_ImplGlfw_Shutdown();
     ImGui::DestroyContext();

     glfwDestroyWindow(window);
     glfwTerminate();

     return 0;
}
