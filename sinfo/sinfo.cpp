#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include "K3Buffer.h"
#include "K3Proc.h"
#include "K3Key.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GLFW/glfw3.h>
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif
#define XVIEW 999
#define YVIEW 666
#define BUFFER_SIZE 600
#define HISTOGRAM_SIZE 100
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

void plotHistogram(K3Buffer* objbuf, const char* name,
                   const char* title = "", const char* siunit = "")
{
     std::vector<float> hist(HISTOGRAM_SIZE, 0);
     float hmin, hmax, hmean, hstdev, bmin, bmax, cur;

     objbuf->build(name, &hist, &hmin, &hmax, &hmean, &hstdev, &bmin, &bmax, &cur);
     const char* overlay = objbuf->overtext(title, cur, hmean, hstdev, siunit, bmin, bmax);

     ImGui::PlotHistogram("", hist.data(), HISTOGRAM_SIZE, 0, overlay, hmin, hmax, ImVec2(screen_width - 16, screen_height / 11));
}

void plotHistory(K3Buffer* objbuf, const char* name,
                 const char* title = "", const char* siunit = "")
{
     ensamble* ens = objbuf->fisher(name);
     std::vector<float>* fector = ens->buffer;
     float min = objbuf->min(fector);
     float max = objbuf->max(fector);
     float cur = fector->back();
     size_t size = fector->size();
     float* duffer = fector->data();
     const char* overlay = objbuf->overtext(title, cur, min, max, siunit, ens->mini, ens->maxi);
     
     ImGui::PlotLines("", duffer, size, 0, overlay, min, max, ImVec2(screen_width - 16, screen_height / 11));
}

//
// Main code
int main(int, char**)
{
     glfwSetErrorCallback(glfw_error_callback);
     if (!glfwInit()) return 1;

     glfw_error_callback(-1, "this is not an error\n");

     // Create window with graphics context
     char sinfo_version[33];
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
     ImGui_ImplGlfw_InitForOpenGL(window, true); // @suppress("Invalid arguments")
     ImGui_ImplOpenGL2_Init(); // @suppress("Invalid arguments")

     static K3Buffer* K3B = new K3Buffer(BUFFER_SIZE);
     static K3Proc* Proc = new K3Proc();
     static K3Key showin(3);
     static bool histogramode = false;          

     static bool do_not_update_system_info = false;
     static bool quit = false;
     static bool dump = false;
     static bool reset = false;

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
          ImGui_ImplOpenGL2_NewFrame(); // @suppress("Invalid arguments")
          ImGui_ImplGlfw_NewFrame(); // @suppress("Invalid arguments")
          ImGui::NewFrame();

          io.FontGlobalScale = font_scale;


          if (ImGui::IsKeyPressed(ImGuiKey_A)) showin.flip(WIN_ABOUT);
          if (ImGui::IsKeyPressed(ImGuiKey_B)) showin.flip(WIN_DEBUG);
          if (ImGui::IsKeyPressed(ImGuiKey_C)) showin.flip(WIN_CONTROL);
          if (ImGui::IsKeyPressed(ImGuiKey_D)) dump = true;
          if (ImGui::IsKeyPressed(ImGuiKey_M)) histogramode = !histogramode;
          if (ImGui::IsKeyPressed(ImGuiKey_Q)) quit = true;
          if (ImGui::IsKeyPressed(ImGuiKey_R)) reset = true;
               
          delay = delay > 0 ? delay : 1;
          do_not_update_system_info = loop % delay;// ? true : false;
          if (!do_not_update_system_info)
          {
               uloop++;
               //Proc->connect();
               Proc->get_sysinfo("totalmem", "freemem", "uptime", "procs");
               Proc->get_statvfs("totalspace", "freespace");
               Proc->processor("cpunumber");
               Proc->connect("cpufreq", "/proc/cpuinfo", "cpu MHz");
               Proc->connect("procstat", "/proc/stat");
               Proc->connect("procloadavg", "/proc/loadavg");
               //unsigned int cpufreq0 = Proc->get_cpufreq_stats(1);
               //glfw_error_callback(cpufreq0, "\n");

               if (uloop < BUFFER_SIZE) status = "load";
               else if (uloop == BUFFER_SIZE) status = "done";
               else status = "run";

               if (strcmp("done", status) == 0) delay = 6;
               float appfreq = 1.0f / io.DeltaTime;
               float upfreq = appfreq / delay;

               buftime = float(BUFFER_SIZE) / upfreq;
                              
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
          //bool* poopen = &boopen;
          //ImGui::ProgressBar(float(proci) / procimax, ImVec2(0, 30), status);


          ImGui::Begin("main", &boopen, mainWindowFlags);

          if (ImGui::SmallButton("[a]bout")) showin.show(WIN_ABOUT, true);
          ImGui::SameLine();
          if (ImGui::SmallButton("de[b]ug")) showin.show(WIN_DEBUG, true);
          ImGui::SameLine();
          if (ImGui::SmallButton("[c]ontrol")) showin.show(WIN_CONTROL, true);
          ImGui::SameLine();
          if (ImGui::SmallButton("[d]ump")) dump = true;
          ImGui::SameLine();
          if (ImGui::SmallButton("[m]ode")) histogramode = !histogramode;
          ImGui::SameLine();
          if (ImGui::SmallButton("[r]eset")) reset = true;
          //ImGui::SameLine();
          //if (ImGui::SmallButton("[q]uit")) quit = true;

          if (histogramode)
          {
               plotHistogram(K3B, "uptime", "uptime", "ssb");
               plotHistogram(K3B, "procs", "total processes");
               plotHistogram(K3B, "loadavg3", "running processes");
               plotHistogram(K3B, "cpunumber", "current processor");
               plotHistogram(K3B, "cpufreq", "cpu frequence", "MHz");
               plotHistogram(K3B, "appfreq", "imgui frequence", "Hz");
               plotHistogram(K3B, "upfreq", "app frequence", "Hz");
               plotHistogram(K3B, "freemem", "free memory", "%");
               plotHistogram(K3B, "freespace", "free storage", "%");
          }
          else
          {
               plotHistory(K3B, "uptime", "uptime", "ssb");
               plotHistory(K3B, "procs", "total processes");
               plotHistory(K3B, "loadavg3", "running processes");
               plotHistory(K3B, "cpunumber", "current processor");
               plotHistory(K3B, "cpufreq", "cpu frequence", "MHz");
               plotHistory(K3B, "appfreq", "imgui frequence", "Hz");
               plotHistory(K3B, "upfreq", "app frequence", "Hz");
               plotHistory(K3B, "freemem", "free memory", "%");
               plotHistory(K3B,  "freespace", "free storage", "%");
          }


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
               //if (ImGui::Button("[a]bout")) showin.show(WIN_ABOUT, true);
               //ImGui::SameLine();
               //if (ImGui::Button("de[b]ug")) showin.show(WIN_DEBUG, true);
               //ImGui::SameLine();
               if (ImGui::Button("[c]lose")) showin.hide();
               //ImGui::SameLine();
               //if (ImGui::Button("[d]ump")) K3B->dump();
               //ImGui::SameLine();
               //if (ImGui::Button("[r]eset")) reset = true;

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
               ImGui::Text("%s", sinfo_version);
               ImGui::SeparatorText("code");
               ImGui::Text(Proc->author());
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

          ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData()); // @suppress("Invalid arguments")

          glfwMakeContextCurrent(window);
          glfwSwapBuffers(window);

          if (quit)
          {
               glfwSetWindowShouldClose(window, 1);
          }
          else if (dump)
          {
               K3B->dump();
               dump = false;
          }
          else if (reset)
          {
               K3B->reset();
               uloop = 0;
               delay = 1;
               showin.hide();
               reset = false;
          }
          else
          {}
     }

     // Cleanup

     delete Proc;
     delete K3B;
     //delete sinfo_version;

     ImGui_ImplOpenGL2_Shutdown(); // @suppress("Invalid arguments")
     ImGui_ImplGlfw_Shutdown(); // @suppress("Invalid arguments")
     ImGui::DestroyContext();

     glfwDestroyWindow(window);
     glfwTerminate();

     return 0;
}


/*
  void oldPlotHistogram(std::vector<float>* buffer, const char* title = "",
  const char* siunit = "", int size = BUFFER_SIZE)
  {
  float last_val = buffer->back();
  float min_val = *std::min_element(buffer->begin(), buffer->end());
  float max_val = *std::max_element(buffer->begin(), buffer->end());
  float bin_width = (max_val - min_val) / HISTOGRAM_SIZE;

  std::vector<float> fector(HISTOGRAM_SIZE, 0);
     
  for (float value : *buffer)
  {
  int bin_index = (int)((value - min_val) / bin_width);
  if (bin_index >= 0 && bin_index < HISTOGRAM_SIZE)
  {
  float old_value = fector[bin_index];
  fector[bin_index] = old_value + 1;
  }
  }

  float min_hist = *std::min_element(fector.begin(), fector.end());
  float max_hist = *std::max_element(fector.begin(), fector.end());

  char overlay[100];
  sprintf(overlay, "%s %9.2f %9.2f %9.2f %s", title, min_val, last_val, max_val, siunit);

  ImGui::PlotHistogram("", fector.data(), HISTOGRAM_SIZE, 0, overlay, min_hist, max_hist, ImVec2(screen_width - 16, screen_height / 11));

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



*/
