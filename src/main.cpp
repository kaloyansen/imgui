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
     if (!glfwInit()) return 1;

     glfw_error_callback(-1, "this is not an error\n");

     // Create window with graphics context
     char* sinfo_version = new char[33];
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
     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
     io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

     ImGui::StyleColorsClassic();
     //ImGuiStyle& style = ImGui::GetStyle();
     //style.ScaleAllSizes(2);
     //ImGui::StyleColorsLight();

     // Setup Platform/Renderer backends
     ImGui_ImplGlfw_InitForOpenGL(window, true);
     ImGui_ImplOpenGL2_Init();

     static K3Buffer* K3B = new K3Buffer(BUFSIZE);
     static K3Proc* Proc = new K3Proc();
     static K3Key showin(3);

     static bool do_not_update_system_info = false;
     static bool quit = false;

     static int procimax = 0;
     static int proci;
     static int loop = 0;
     static int uloop = 0;
     static int delay = 1;
     static float buftime;
     static float font_scale = 2.5;
     static const char* status = "unknown";

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
          if (ImGui::IsKeyPressed(ImGuiKey_Q)) quit = true;
          if (ImGui::IsKeyPressed(ImGuiKey_R))
          {
               K3B->reset();
               uloop = 0;
               delay = 1;
               showin.hide();
          }
               
          delay = delay > 0 ? delay : 1;
          do_not_update_system_info = loop % delay;// ? true : false;
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
//          bool* poopen = &boopen;

          ImGui::Begin("main", &boopen, mainWindowFlags);

          if (ImGui::SmallButton("[c]ontrol")) showin.show(WIN_CONTROL, true);
          ImGui::SameLine();
          ImGui::ProgressBar(float(proci) / procimax, ImVec2(0, 30), status);

          ImGui::SameLine();
          if (ImGui::SmallButton("[q]uit")) quit = true;

          plotBuffer(K3B->get("uptime"), "system uptime", "ssb");
          plotBuffer(K3B->get("procs"), "total processes");
          plotBuffer(K3B->get("loadavg3"), "running processes");
          plotBuffer(K3B->get("cpunumber"), "current processor");
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
               if (ImGui::Button("[a]ll right")) showin.show(WIN_ABOUT, false);
               ImGui::End();
          }


// Rendering
          ImGui::Render();
          int display_w, display_h;
          glfwGetFramebufferSize(window, &display_w, &display_h);
          glViewport(0, 0, display_w, display_h);
          //glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
          glClear(GL_COLOR_BUFFER_BIT);

          ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

          glfwMakeContextCurrent(window);
          glfwSwapBuffers(window);

          if (quit) glfwSetWindowShouldClose(window, 1);
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
