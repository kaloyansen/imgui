// Dear ImGui: standalone example application for GLFW + OpenGL2, using legacy fixed pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// **DO NOT USE THIS CODE IF YOUR CODE/ENGINE IS USING MODERN OPENGL (SHADERS, VBO, VAO, etc.)**
// **Prefer using the code in the example_glfw_opengl2/ folder**
// See imgui_impl_glfw.cpp for details.

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
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

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
     fprintf(stderr, "GLFW Error %d: %s\n", error, description);
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
//               glfw_error_callback(1, token[i]);
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

void loaData(float plotData[], int arrSize, float newData)
{
     for (int i = 0; i < arrSize; i++) plotData[i] = plotData[i + 1];
     plotData[arrSize - 1] = newData;
}

float* randoBuffer(int size)
{
     float* buffer = new float[size];
     for (int i = 0; i < size; i++)
          buffer[i] = static_cast<float>(rand()) / RAND_MAX;
     return buffer;
}

//
// Main code
int main(int, char**)
{
     glfwSetErrorCallback(glfw_error_callback);
     if (!glfwInit())
          return 1;

     // Create window with graphics context
     GLFWwindow* window = glfwCreateWindow(1280, 720, "sinfo", nullptr, nullptr);
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
     ImGui::StyleColorsDark();
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
     bool show_another_window = false;
     bool do_not_update_system_info = false;
     ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);



     long long totalMem, freeMem, totalSpace, freeSpace;
     double loadavgarr[7] = {0.0};
     float cpufreq;
     std::string procloadavg;
     std::string procstat;
     int procimax = 0;
     int bufSize = 333;
     float bufloadavg0[bufSize];
     float bufloadavg1[bufSize];
     float bufloadavg2[bufSize];
     float bufloadavg3[bufSize];
     float* bufcpufreq = randoBuffer(bufSize);
     float* bufappfreq = randoBuffer(bufSize);
     float* bufupfreq = randoBuffer(bufSize);

     int loop = 0;
     int uloop = 0;
     const char* rose = "calibrate";
     int delay = 1;
     float fontScale = 2.0f;

     // Main loop
     while (!glfwWindowShouldClose(window))
     {
          loop++;
          // Poll and handle events (inputs, window resize, etc.)
          // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
          // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
          // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
          // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
          glfwPollEvents();

          // Start the Dear ImGui frame
          ImGui_ImplOpenGL2_NewFrame();
          ImGui_ImplGlfw_NewFrame();
          ImGui::NewFrame();
          //static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

          // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
          if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

          // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
          {
               do_not_update_system_info = loop % delay ? true : false;
               if (!do_not_update_system_info)
               {
                    uloop++;
                    updateSystemInfo(&totalMem, &freeMem, &totalSpace, &freeSpace, loadavgarr, &cpufreq,  &procloadavg,  &procstat);
               }

               float appfreq = 1.0f / io.DeltaTime;
               float upfreq = appfreq / delay;
                              
               float ram = 100 * float(freeMem) / float(totalMem);
               float rom = 100 * float(freeSpace) / float(totalSpace);
               int proci = int(loadavgarr[3]);
               procimax = proci > procimax ? proci : proci * 2;
               //if (counter > 0) rose = "yeah, thank you";

               if (!do_not_update_system_info)
               {
                    loaData(bufloadavg0, bufSize, loadavgarr[0]);
                    loaData(bufloadavg1, bufSize, loadavgarr[1]);
                    loaData(bufloadavg2, bufSize, loadavgarr[2]);
                    loaData(bufloadavg3, bufSize, loadavgarr[3]);
                    loaData(bufcpufreq, bufSize, cpufreq);
                    loaData(bufappfreq, bufSize, appfreq);
                    loaData(bufupfreq, bufSize, upfreq);
               }

               // for (int i = 0; i < IM_ARRAYSIZE(plotData); i++)
               // {
               //      plotData[i - 1] = plotData[i];
               // }
               // plotData[bufSize - 1] = loadavgarr[3];

               io.FontGlobalScale = fontScale;

               static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
               
               const ImGuiViewport* viewport = ImGui::GetMainViewport();
               ImGui::SetNextWindowPos(viewport->WorkPos);
               ImGui::SetNextWindowSize(viewport->WorkSize);

               bool boopen = true;
               bool* poopen = &boopen;

               ImGui::Begin("main", poopen, flags);
//               ImGui::CheckboxFlags("ImGuiWindowFlags_NoTitleBar", &flags, ImGuiWindowFlags_NoTitleBar);
//               ImGui::CheckboxFlags("ImGuiWindowFlags_NoCollapse", &flags, ImGuiWindowFlags_NoCollapse);
//               ImGui::CheckboxFlags("ImGuiWindowFlags_NoScrollbar", &flags, ImGuiWindowFlags_NoScrollbar);
               ImGui::SliderFloat("scale", &fontScale, 0.5, 5);
               ImGui::ShowStyleSelector("style");
               ImGui::SliderInt("delay", &delay, 1, 10);
               ImGui::Separator();

               //ImGui::Checkbox("pause", &do_not_update_system_info);
               //ImGui::ColorEdit3("clear color", (float*)&clear_color);

/* buttons return true when clicked like most widgets when edited or activated */
               
               //ImGui::SliderFloat("RAM", &ram, 0.0f, 100.0f);
               //ImGui::SameLine();
               //ImGui::Text("my counter is %d", counter);

               //ImGui::SliderFloat("frequence, Hz", &frequence, 1, 120);
               ImGui::ProgressBar(upfreq / 100, ImVec2(0.0f, 0.0f));
               ImGui::SameLine();
               ImGui::Text("update frequence %.1f Hz", upfreq);
               ImGui::ProgressBar(ram / 100, ImVec2(0.0f, 0.0f));
               ImGui::SameLine();
               ImGui::Text("RAM %d/%d Mo", int(freeMem / 1024 / 1024), int(totalMem / 1024 / 1024));

               //ImGui::SliderFloat("ROM", &rom, 0.0f, 100.0f);
               ImGui::ProgressBar(rom / 100, ImVec2(0.0f, 0.0f));
               ImGui::SameLine();
               ImGui::Text("ROM %d/%d Go", int(freeSpace / 1024 / 1024 / 1024), int(totalSpace / 1024 / 1024 /1024));

               ImGui::Separator();
               if (ImGui::BeginTable("split", 5, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
               {
                    //sprintf(buf, "%03d", i);
                    ImGui::TableNextColumn();
                    ImGui::Text("app avg rate, s");
                    ImGui::TableNextColumn();
                    ImGui::Text("FPS");
                    ImGui::TableNextColumn();
                    ImGui::Text("updates");
                    ImGui::TableNextColumn();
                    ImGui::Text("loops");
                    ImGui::TableNextColumn();
                    ImGui::Text("loops/60");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%.6f", 1.0f / io.Framerate);
                    ImGui::TableNextColumn();
                    ImGui::Text("%.1f", io.Framerate);
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", uloop);
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", loop);
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", loop / 60);
                    ImGui::EndTable();
               }

               ImGui::SliderInt("processed", &proci, 0, procimax);
               // if (uloop > bufSize)
               // {
               ImGui::SeparatorText("ImGui::SeparaorText");
               if (ImGui::Button(rose)) uloop = 0;

               char overlay[32];
               sprintf(overlay, "%12.0f Hz", cpufreq * 1000 * 1000);
               //ImGui::SetNextItemHeight(ImGui::GetFontSize() * 8);
               ImGui::PlotLines("core frequence", bufcpufreq, bufSize, 0, overlay);

               ImGui::PlotLines("imgui frequence", bufappfreq, bufSize);
               ImGui::SameLine();
               ImGui::Text("%7.1f Hz", appfreq);

               ImGui::PlotLines("update frequence", bufupfreq, bufSize);
               ImGui::SameLine();
               ImGui::Text("%7.1f Hz", upfreq);
                    
               ImGui::PlotLines("current load", bufloadavg3, bufSize);
               ImGui::PlotLines("01 min load avg", bufloadavg0, bufSize);
               ImGui::PlotLines("05 min load avg", bufloadavg1, bufSize);
               ImGui::PlotLines("15 min load avg", bufloadavg2, bufSize);
               // }
               // else
               // {
               //      int progress = 100 - (100 * float(bufSize - uloop) / bufSize);
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


               ImGui::Indent();
               ImGui::Text("debug");
               ImGui::Text("loadavg: %s", procloadavg.c_str());
               ImGui::Text("stat: %s", procstat.c_str());

               ImGui::Text("%f", cpufreq);

               ImGui::Checkbox("Demo Window", &show_demo_window);
               ImGui::Checkbox("Another Window", &show_another_window);
//               ImGui::Unindent();

               if (ImGui::IsKeyDown(ImGuiKey_Q)) ImGui::Text("yeah");
               if (poopen && ImGui::Button("Close this window")) boopen = false;
               ImGui::End();
          }

          // 3. Show another simple window.
          if (show_another_window)
          {
               ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
             
               ImGui::Text("Hello from another window!");
               if (ImGui::Button("Close Me"))
                    show_another_window = false;
               ImGui::End();
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
     delete bufcpufreq;
     delete bufappfreq;
     delete bufupfreq;

     ImGui_ImplOpenGL2_Shutdown();
     ImGui_ImplGlfw_Shutdown();
     ImGui::DestroyContext();

     glfwDestroyWindow(window);
     glfwTerminate();

     return 0;
}
