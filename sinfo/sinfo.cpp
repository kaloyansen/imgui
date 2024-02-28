#include "sinfo.h"

int screen_width;
int screen_height;
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

          static bool histogram = true; 
          static bool history = false; 
          if (histogramode)
          {
               draw(K3B, "procs",     "total processes",   "", histogram);
               draw(K3B, "loadavg3",  "running processes", "", histogram);
               draw(K3B, "cpunumber", "current processor", "", histogram);
               draw(K3B, "cpufreq",   "cpu frequence",  "MHz", history);
               draw(K3B, "appfreq",   "imgui frequence", "Hz", history);
               draw(K3B, "upfreq",    "app frequence",   "Hz", history);
               draw(K3B, "uptime",    "uptime",         "ssb", histogram);
               draw(K3B, "freemem",   "free memory",      "%", history);
               draw(K3B, "freespace", "free storage",     "%", histogram);
          }
          else
          {
               draw(K3B, "procs",     "total processes",   "", history);
               draw(K3B, "loadavg3",  "running processes", "", history);
               draw(K3B, "cpunumber", "current processor", "", history);
               draw(K3B, "cpufreq",   "cpu frequence",  "MHz", histogram);
               draw(K3B, "appfreq",   "imgui frequence", "Hz", histogram);
               draw(K3B, "upfreq",    "app frequence",   "Hz", histogram);
               draw(K3B, "uptime",    "uptime",         "ssb", history);
               draw(K3B, "freemem",   "free memory",      "%", histogram);
               draw(K3B, "freespace", "free storage",     "%", history);
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
               if (ImGui::Button("[c]lose")) showin.hide();

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
               ImGui::Text("ImGui, GLFW, OpenGL, X11");
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

     ImGui_ImplOpenGL2_Shutdown(); // @suppress("Invalid arguments")
     ImGui_ImplGlfw_Shutdown(); // @suppress("Invalid arguments")
     ImGui::DestroyContext();

     glfwDestroyWindow(window);
     glfwTerminate();

     return 0;
}



static ImVec2 plain(void)
{
     int* vjhbiyg = &screen_width;
     int* hjjhvgf = &screen_height;
     ImVec2 p(*vjhbiyg - 16, *hjjhvgf / 11);
     return p;
}

static void draw(K3Buffer* objbuf, const char* name,
                 const char* title, const char* siunit,
                 bool mode)
{
     if (mode) plotHistogram(objbuf, name, title, siunit);
     else plotHistory(objbuf, name, title, siunit);    

}



static void plotHistogram(K3Buffer* objbuf, const char* name,
                   const char* title = "", const char* siunit = "")
{
     std::vector<float> hist(HISTOGRAM_SIZE, 0);
     float hmin, hmax, hmean, hstdev, bmin, bmax, cur;

     objbuf->build(name, &hist, &hmin, &hmax, &hmean, &hstdev, &bmin, &bmax, &cur);
     const char* overlay = objbuf->overtext(title, cur, hmean, hstdev, siunit, bmin, bmax);

     ImGui::PlotHistogram("", hist.data(), HISTOGRAM_SIZE, 0, overlay, hmin, hmax, plain());
}

static void plotHistory(K3Buffer* objbuf, const char* name,
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
     
     ImGui::PlotLines("", duffer, size, 0, overlay, min, max, plain());
}

static void glfw_error_callback(int error, const char* description)
{
     printf("glfw %d %s\n", error, description);
}
