#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include "K3Buffer.h"
#include "K3Proc.h"
#include "K3Key.h"
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
#define VERSION "0.0.3"
#define WIN_ABOUT 0
#define WIN_DEBUG 1
#define WIN_CONTROL 2

///////////////////////////////////////////////////////////////////////////////////
int screen_width;
int screen_height;

static void glfw_error_callback(int error, const char* description)
{
     fprintf(stderr, "glfw %d %s\n", error, description);
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
     if (!glfwInit()) return 1;

     glfw_error_callback(-1, "this is not an error\n");

     // Create window with graphics context
     char sinfo_version[32];// = new char[111];
     sprintf(sinfo_version, "sinfo v%s", VERSION);
     GLFWwindow* window = glfwCreateWindow(XVIEW, YVIEW, sinfo_version, nullptr, nullptr);
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

     K3Buffer* K3B = new K3Buffer(BUFSIZE);
     K3Proc* Proc = new K3Proc();
     K3Key showin(3);

     //showin.hide();



     bool do_not_update_system_info = false;
     bool quit = false;
     ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

     int procimax = 0;
     int proci;
     

     int loop = 0;
     int uloop = 0;
     int delay = 1;
     float buftime;
     float font_scale = 1.7f;
     const char* status = "unknown";

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


          if (ImGui::IsKeyPressed(ImGuiKey_A)) showin.flip(WIN_ABOUT);
          if (ImGui::IsKeyPressed(ImGuiKey_B)) showin.flip(WIN_DEBUG);
          if (ImGui::IsKeyPressed(ImGuiKey_C)) showin.flip(WIN_CONTROL);
          if (ImGui::IsKeyPressed(ImGuiKey_D)) K3B->dump();
          if (ImGui::IsKeyPressed(ImGuiKey_R))
          {
               K3B->reset();
               uloop = 0;
               delay = 1;
               showin.hide();
          }
               
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
               float appfreq = 1.0f / io.DeltaTime;
               float upfreq = appfreq / delay;

               buftime = float(BUFSIZE) / upfreq;
                              
               proci = Proc->get("procloadavg")->valeur[3];
               procimax = proci > procimax ? proci : procimax;

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

          const ImGuiViewport* viewport = ImGui::GetMainViewport();
          ImGui::SetNextWindowPos(viewport->WorkPos);
          ImGui::SetNextWindowSize(viewport->WorkSize);
          screen_width = viewport->WorkSize.x;
          screen_height = viewport->WorkSize.y;

          bool  boopen = true;
          bool* poopen = &boopen;

          ImGui::Begin("main", poopen, mainWindowFlags);

          if (ImGui::SmallButton("[c]ontrol")) showin.show(WIN_CONTROL, true);
          ImGui::SameLine();
          ImGui::ProgressBar(float(proci) / procimax, ImVec2(0, 22), status);

          ImGui::SameLine();
          if (ImGui::SmallButton("[q]uit")) quit = true;

          plotBuffer(K3B->get("uptime"), "system uptime", "ssb");
          plotBuffer(K3B->get("procs"), "total processes");
          plotBuffer(K3B->get("loadavg3"), "running processes");
          plotBuffer(K3B->get("cpunumber"), "cpu number");
          plotBuffer(K3B->get("cpufreq"), "cpu frequence", "MHz");
          plotBuffer(K3B->get("appfreq"), "imgui frequence", "Hz");
          plotBuffer(K3B->get("upfreq"), "app frequence", "Hz");
          plotBuffer(K3B->get("freemem"), "free memory", "%");
          plotBuffer(K3B->get("freespace"), "free storage", "%");

          ImGui::End();
          

          if (showin.status(WIN_DEBUG) && ImGui::Begin("debug", showin.is(WIN_DEBUG), controlWindowFlags))
          {
               ImGui::SeparatorText("debug");
               ImGui::Text("loadavg: %s", Proc->get("procloadavg")->text);
               ImGui::Text("stat: %s", Proc->get("procstat")->text);
               ImGui::End();
          }

          if (showin.status(WIN_CONTROL) && ImGui::Begin("control", showin.is(WIN_CONTROL), controlWindowFlags))
          {
               if (ImGui::Button("[a]bout")) showin.show(WIN_ABOUT, true);
               ImGui::SameLine();
               if (ImGui::Button("de[b]ug")) showin.show(WIN_DEBUG, true);
               ImGui::SameLine();
               if (ImGui::Button("[c]lose")) showin.hide();
               ImGui::SameLine();
               if (ImGui::Button("[d]ump")) K3B->dump();
               ImGui::SameLine();
               if (ImGui::Button("[r]eset"))
               {
                    K3B->reset();
                    uloop = 0;
                    delay = 1;
                    showin.hide();
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


          if (showin.status(WIN_ABOUT) && ImGui::Begin("about", showin.is(WIN_ABOUT), controlWindowFlags))
          {
               ImGui::Text(sinfo_version);
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
               if (ImGui::Button("c[a]ncel")) showin.show(WIN_ABOUT, false);
               ImGui::End();
          }


// Rendering
          ImGui::Render();
          int display_w, display_h;
          glfwGetFramebufferSize(window, &display_w, &display_h);
          glViewport(0, 0, display_w, display_h);
          glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
          glClear(GL_COLOR_BUFFER_BIT);

          ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

          glfwMakeContextCurrent(window);
          glfwSwapBuffers(window);

          if (quit || ImGui::IsKeyPressed(ImGuiKey_Q)) glfwSetWindowShouldClose(window, 1);
     }

     // Cleanup

     delete Proc;
     delete K3B;

     ImGui_ImplOpenGL2_Shutdown();
     ImGui_ImplGlfw_Shutdown();
     ImGui::DestroyContext();

     glfwDestroyWindow(window);
     glfwTerminate();

     return 0;
}
