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
#define DEBUG false
#define VERSION 0.2

int screen_width;
int screen_height;

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


int getDirectiveValue(const char* path, const char* directive)
{
     FILE* file = fopen(path, "r");
     if (file == NULL) return -1;

     char line[256];
     double valeur = -1.0;

     while (fgets(line, sizeof(line), file) != NULL) {
          if (strstr(line, directive)) {
               char* token = strtok(line, ":");
               if (token != NULL) {
                    token = strtok(NULL, ":");
                    if (token != NULL) {
                         valeur = atof(token);
                         break;
                    }
               }
          }
     }

     fclose(file);

     return valeur;
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
     *cpufreq = getDirectiveValue("/proc/cpuinfo", "cpu MHz");//getCPUUsage();

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

void processBuffer(std::vector<float>* buffer, float* min, float* max)
{
     auto amin = min_element(buffer->begin(), buffer->end());
     auto amax = max_element(buffer->begin(), buffer->end());
     *min = float(*amin);
     *max = float(*amax);
}

void plotBuffer(std::vector<float>* buffer, const char* title = "",
                const char* unit = "", int size = BUFSIZE)
{
     float last = buffer->back();
     char overlay[100];
     float min, max;
     float* pmin = &min;
     float* pmax = &max;

     //processBuffer(buffer, pmin, pmax);
     K3Buffer delme(0);
     delme.process(buffer, pmin, pmax);

     sprintf(overlay, "%s %9.2f %9.2f %9.2f %s", title, min, last, max, unit);

     ImGui::PlotLines("", buffer->data(), size, 0, overlay, min, max, ImVec2(screen_width, screen_height / 7));
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

     // Load Fonts
     // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
     // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
     // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
     // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
     // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
     // - Read 'docs/FONTS.md' for more instructions and details.
     // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
     //io.Fonts->AddFontDefault();
     //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
     //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
     //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
     //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
     //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
     //IM_ASSERT(font != nullptr);

     // Our state
     bool show_demo_window = false;
     bool show_control = false;
     bool show_about = false;
     bool show_quit = false;
     bool do_not_update_system_info = false;
     ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

     long long totalMem, freeMem, totalSpace, freeSpace;
     double loadavgarr[7] = {0.0};
     float cpufreq;
     std::string procloadavg;
     std::string procstat;
     int procimax = 0;

     K3Buffer* K3B = new K3Buffer(BUFSIZE);
     K3B->append("loadavg0", "loadavg1", "loadavg2", "loadavg3", "cpufreq", "appfreq", "upfreq", "freemem", "freespace", nullptr);

     K3Proc* Proc = new K3Proc();
     delete Proc;
     

     int loop = 0;
     int uloop = 0;
     int delay = 1;
     float buftime;
     float font_scale = 1.7f;
     const char* status = "unknown";

     static ImGuiWindowFlags mainWindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
     static ImGuiWindowFlags controlWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
     static ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_SizingFixedFit;
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


          // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
          if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

          // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
          {
               delay = delay > 0 ? delay : 1;
               do_not_update_system_info = loop % delay ? true : false;
               if (!do_not_update_system_info)
               {
                    uloop++;
                    updateSystemInfo(&totalMem, &freeMem, &totalSpace, &freeSpace, loadavgarr, &cpufreq,  &procloadavg,  &procstat);
               }

               if (uloop < BUFSIZE) status = "load";
               else if (uloop == BUFSIZE) status = "done";
               else status = "run";

               if (strcmp("done", status) == 0) delay = 6;
//               if (status == "done") delay = 6;
               float appfreq = 1.0f / io.DeltaTime;
               float upfreq = appfreq / delay;
               buftime = float(BUFSIZE) / upfreq;
                              
               float ram = 100 * float(freeMem) / float(totalMem);
               float rom = 100 * float(freeSpace) / float(totalSpace);
               int proci = int(loadavgarr[3]);
               procimax = proci > procimax ? proci : proci * 2;

               if (!do_not_update_system_info)
               {
                    K3B->fill("cpufreq", cpufreq);
                    K3B->fill("freespace", rom);
                    K3B->fill("freemem", ram);
                    K3B->fill("upfreq", upfreq);
                    K3B->fill("appfreq", appfreq);
                    K3B->fill("loadavg0", loadavgarr[0]);
                    K3B->fill("loadavg1", loadavgarr[1]);
                    K3B->fill("loadavg2", loadavgarr[2]);
                    K3B->fill("loadavg3", loadavgarr[3]);
               }

               if (ImGui::IsKeyPressed(ImGuiKey_D)) K3B->dump();
               if (ImGui::IsKeyPressed(ImGuiKey_R))
               {
                    K3B->reset();
                    uloop = 0;
                    delay = 1;
               }

// for (int i = 0; i < IM_ARRAYSIZE(plotData); i++)
               // {
               //      plotData[i - 1] = plotData[i];
               // }
               // plotData[BUFSIZE - 1] = loadavgarr[3];

               
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



               // if (ImGui::BeginMenu("control"))
               // {
               //      ImGui::MenuItem("hmmmm", NULL, &show_control);
               //      ImGui::EndMenu();
               // }


               if (ImGui::BeginTable("system", 6, tableFlags, ImVec2(800, 0)))
               {
                    //sprintf(buf, "%03d", i);
                    ImGui::TableNextColumn();
                    ImGui::Text("sinfo v%3.1f", VERSION);
                    ImGui::TableNextColumn();
                    ImGui::Text("rate, s");
                    ImGui::TableNextColumn();
                    ImGui::Text("FPS");
                    ImGui::TableNextColumn();
                    ImGui::Text("updates");
                    ImGui::TableNextColumn();
                    ImGui::Text("loops/60");
//                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::ProgressBar(0.1f * proci, ImVec2(0.0f, 0.0f), status);
                    // ImGui::TableNextColumn();
                    // ImGui::Text("history:");
                    // ImGui::TableNextColumn();
                    // ImGui::Text("%6.1f s", buftime);
                    ImGui::TableNextColumn();

                    if (show_control || show_about)
                    {
                         if (ImGui::Button("[c]lose") || ImGui::IsKeyPressed(ImGuiKey_C))
                              show_control = false;
                    }
                    else
                    {
                         if (ImGui::Button("[k]ontrol") || ImGui::IsKeyPressed(ImGuiKey_K))
                              show_control = true;
                    }
                    
                    ImGui::TableNextColumn();
                    ImGui::Text("%.3f", 1.0f / io.Framerate);
                    ImGui::TableNextColumn();
                    ImGui::Text("%.1f", io.Framerate);
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", uloop);
                    ImGui::TableNextColumn();
                    ImGui::Text("%d/%d", loop, loop / 60);
                    ImGui::TableNextColumn();
                    if (ImGui::IsKeyPressed(ImGuiKey_Q) || ImGui::Button("[q]uit"))
                         show_quit = true;
//                         glfwSetWindowShouldClose(window, 1);
                    ImGui::EndTable();
               }

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
               plotBuffer(K3B->get("cpufreq"), "processor frequence", "MHz");
               plotBuffer(K3B->get("appfreq"), "imgui frequence", "Hz");
               plotBuffer(K3B->get("upfreq"), "app frequence", "Hz");
               plotBuffer(K3B->get("freemem"), "free memory", "%");
               plotBuffer(K3B->get("freespace"), "free storage", "%");



// ImGui::PlotLines("", bufcpufreq, BUFSIZE, BUFSIZE,
               //                  "processor frequence, MHz");
               // ImGui::SameLine();
               // sprintf(overlay, "%7.1f", cpufreq);
               // ImGui::ProgressBar(cpufreq / 1111, ImVec2(333.0f, 0.0f), overlay);
               

               // sprintf(overlay, "update frequence %7.1f Hz", upfreq);
               // ImGui::ProgressBar(upfreq / 100, ImVec2(0.0f, 50.0f), overlay);

               /*
                 sprintf(overlay, "RAM %d/%d Mo", int(freeMem / 1024 / 1024), int(totalMem / 1024 / 1024));
                 ImGui::ProgressBar(ram / 100, ImVec2(0.0f, 50.0f), overlay);
//               ImGui::SameLine();

sprintf(overlay, "ROM %d/%d Go", int(freeSpace / 1024 / 1024 / 1024), int(totalSpace / 1024 / 1024 /1024));
ImGui::ProgressBar(rom / 100, ImVec2(0.0f, 50.0f), overlay);

ImGui::Separator();

ImGui::SliderInt("processed", &proci, 0, procimax);
               */


// if (uloop > BUFSIZE)
               // {

/*               ImGui::SeparatorText("plots");

                 sprintf(overlay, "%12.0f Hz", cpufreq * 1000 * 1000);
                 ImGui::PlotLines("core frequence", bufcpufreq, BUFSIZE, 0, overlay);

                 ImGui::PlotLines("imgui frequence", bufappfreq, BUFSIZE);
                 ImGui::Text("%7.1f Hz", appfreq);

                 ImGui::PlotLines("update frequence", bufupfreq, BUFSIZE);
                 ImGui::Text("%7.1f Hz", upfreq);
                    
                 ImGui::PlotLines("current load", bufloadavg3, BUFSIZE);
                 ImGui::PlotLines("01 min load avg", bufloadavg0, BUFSIZE);
                 ImGui::PlotLines("05 min load avg", bufloadavg1, BUFSIZE);
                 ImGui::PlotLines("15 min load avg", bufloadavg2, BUFSIZE);
*/
               // }
               // else
               // {
               //      int progress = 100 - (100 * float(BUFSIZE - uloop) / BUFSIZE);
               //      ImGui::Text("loading ...");
               //      ImGui::Text("calibrate process load");
               //      ImGui::SameLine();
               //      ImGui::ProgressBar(float(progress)/100, ImVec2(0.0f, 0.0f));
               //      ImGui::Text("calculate one min average");
               //      ImGui::SameLine();
               //      ImGui::ProgressBar(float(progress)/100, ImVec2(0.0f, 0.0f));
               //      ImGui::Text("calculate five min average");
               //      ImGui::SameLine();
               //      ImGui::ProgressBar(float(progress)/100, ImVec2(0.0f, 0.0f));
               //      ImGui::Text("calculate fifteen min acerage");
               //      ImGui::SameLine();
               //      ImGui::ProgressBar(float(progress)/100, ImVec2(0.0f, 0.0f));
               // }


//               ImGui::Indent();

               if (DEBUG)
               {
                    ImGui::SeparatorText("debug");
                    ImGui::Text("loadavg: %s", procloadavg.c_str());
                    ImGui::Text("stat: %s", procstat.c_str());
               }

               //ImGui::Checkbox("demo", &show_demo_window);
               //ImGui::Checkbox("control", &show_control);
//               ImGui::Unindent();

               //*poopen = false;
//               if (poopen && ImGui::Button("quit")) boopen = false;
               ImGui::End();
          }

          if (show_control)
          {
               if (ImGui::Begin("control", &show_control, controlWindowFlags))
               {
                    if (ImGui::Button("[a]bout") || ImGui::IsKeyPressed(ImGuiKey_A))
                         show_about = true;
                    ImGui::SameLine();
                    if (ImGui::Button("[c]lose") || ImGui::IsKeyPressed(ImGuiKey_C))
                         show_control = false;
                    // ImGui::SameLine();
                    // if (ImGui::Button("[d]ump") || ImGui::IsKeyPressed(ImGuiKey_D))
                    //      K3B->dump();
                    // ImGui::SameLine();
                    // if (ImGui::Button("[r]eset") || ImGui::IsKeyPressed(ImGuiKey_R))
                    // {
                    //      K3B->reset();
                    //      uloop = 0;
                    //      delay = 1;
                    // }
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
               show_control = false;
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
                    if (ImGui::Button("[c]lose") || ImGui::IsKeyPressed(ImGuiKey_C))
                         show_about = false;
                    ImGui::End();
               }
          }

          if (show_quit)
          {
               if (ImGui::Begin("quit", &show_quit, controlWindowFlags))
               {
                    ImGui::Text("sinfo v%3.1f", VERSION);
                    ImGui::Text("really quit ?");
                    if (ImGui::Button("[y]es") || ImGui::IsKeyPressed(ImGuiKey_Y))
                    {
                         delete K3B;
                         glfwSetWindowShouldClose(window, 1);
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("[c]ancel") || ImGui::IsKeyPressed(ImGuiKey_C))
                         show_quit = false;
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
