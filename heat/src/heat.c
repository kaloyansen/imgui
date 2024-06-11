#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include "implot.h"
#include "implot_internal.h"

#include <stdio.h>
#include <stdarg.h>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GLFW/glfw3.h>
#include "buf.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#define XVIEW 999
#define YVIEW 333
#define DEBUG true
#define VERSION "0.0.3"
#define WIN_ABOUT 0
#define WIN_DEBUG 1
#define WIN_CONTROL 2
#define HISTO_SIZE 1e1
#define HIGH_TEMPERATURE 77
#define LOW_TEMPERATURE 66


int screen_width;
int screen_height;

static void glfw_error_callback(int error, const char* description)
{
     fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

/* static ImVec2 plain(void) */
/* { */
/*      int* vjhbiyg = &screen_width; */
/*      int* hjjhvgf = &screen_height; */
/*      ImVec2 p(*vjhbiyg - 16, *hjjhvgf / 6); */
/*      return p; */
/* } */

ImU32 Couleur(const char * colorName)
{
     int c = 255;
     if (strcmp(colorName, "red") == 0) return IM_COL32(c, 0, 0, c);
     else if (strcmp(colorName, "green")   == 0) return IM_COL32(0, c, 0, c);
     else if (strcmp(colorName, "blue")    == 0) return IM_COL32(0, 0, c, c);
     else if (strcmp(colorName, "yellow")  == 0) return IM_COL32(c, c, 0, c);
     else if (strcmp(colorName, "cyan")    == 0) return IM_COL32(0, c, c, c);
     else if (strcmp(colorName, "magenta") == 0) return IM_COL32(c, 0, c, c);
     else if (strcmp(colorName, "black")   == 0) return IM_COL32(0, 0, 0, c);
     else if (strcmp(colorName, "white")   == 0) return IM_COL32(c, c, c, c);
     else if (strcmp(colorName, "gray")    == 0) return IM_COL32(c / 2, c / 2, c / 2, c);
     else if (strcmp(colorName, "orange")  == 0) return IM_COL32(c, 165, 0, c);
     else return IM_COL32(c, c, c, c);
}




const char* formatString(const char *format, ...)
{
     va_list args;
     va_start(args, format);
     int size = vsnprintf(NULL, 0, format, args) + 1; // +1 for terminator
     va_end(args);

     char * mot = (char *)malloc(size * sizeof(char));
     if (mot == NULL) {
          glfw_error_callback(111, "Memory allocation failed\n");
          exit(EXIT_FAILURE);
          return NULL;
     }

     va_start(args, format);
     vsnprintf(mot, size, format, args);
     va_end(args);

     return mot;
}

/* static void spacePlot(const CircularBuffer cb, const char* title = "", const char* siunit = "") */
/* { */
/*      float hmin = 0; */
/*      float hmax = 0; */
/*      float hsize = 0; */
/*      float * hist = histogram(&cb, HISTO_SIZE, &hmin, &hmax, &hsize); */
/*      const char * overlay = formatString("%s %.0f", title, hsize); */
/*      ImGui::PlotHistogram("", hist, hsize, 0, overlay, hmin, hmax, plain()); */
/*      if (hist) free((void *)hist); */
/* } */

static void timePlot(CircularBuffer cb[],
                     const char* title = "", const char* siunit = "")
{
     //const char * lay = formatString("%6.1f [%6.1f ; %6.1f]", cb.last, cb.min, cb.max);
     //const char * overlay = formatString("%s %s %s", title, lay, siunit);
     int arrsize = 4;//sizeof(cb) / sizeof(cb[0]);
     float ymin = cb[0].min;
     float ymax = cb[0].max;
     for (int i = 1; i < arrsize; i ++)
     {
          ymin = ymin < cb[i].min ? ymin : cb[i].min;
          ymax = ymax > cb[i].max ? ymax : cb[i].max;
     }

     if (ymax > HIGH_TEMPERATURE) ImGui::StyleColorsLight();
     if (ymax < LOW_TEMPERATURE) ImGui::StyleColorsDark();
     
     ImPlot::SetNextAxesLimits(-1 * (float)cb[0].size, 0.0f, ymin, ymax, ImGuiCond_Always);
     ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 3.0f);
     ImU32 col[] = {Couleur("magenta"), Couleur("cyan"), Couleur("yellow"), Couleur("red")};
     ImVec2 full_size = ImGui::GetContentRegionAvail();
     if (ImPlot::BeginPlot("https://kaloyansen.github.io", ImVec2(-1, full_size.y)))
          //if (ImPlot::BeginPlot("kaloyansen.github.io"))
     {
          /* ImPlot::SetupAxis(ImAxis_Y2, "temperature/°C", ImPlotAxisFlags_AuxDefault); */
          /* ImPlot::SetupAxisLimits(ImAxis_Y2, ymin, ymax); */
          /* ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2); */
          for (int i = 0; i < arrsize; i ++)
          {
               const char * title = formatString("core %d %.1f", i, cb[i].last);
               const float * data = cb[i].data;
               const size_t size = cb[i].size;
               
               ImPlot::PushStyleColor(ImPlotCol_Line, col[i]);
               float time[size];
               for (size_t j = 0; j < size; j ++)
               {
                    time[j] = -1 * (float)cb[i].size + (float)j;
               }               

               //ImPlot::PlotLine(title, time, data, size);
               ImPlot::PlotLine(title, time, data, size);
               if (title) free((void *)title);
          }

          ImPlot::EndPlot();
     }
     //if (overlay != NULL) free((void *)overlay);
     //if (lay != NULL) free((void *)lay);
}

/* static void draw(const CircularBuffer objbuf, const char* title, const char* siunit, bool mode = 0) */
/* { */
/*      if (mode) spacePlot(objbuf, title, siunit); */
/*      else timePlot(objbuf, objbuf, objbuf, objbuf, title, siunit); */
/* } */


static void presentation(CircularBuffer cb[], int number_of_threads, bool mode = 0)
{
     for (int i = 0; i < number_of_threads; i ++) pthread_mutex_lock(&cb[i].mutex);
     ImGui::Text("%zu %zus %zus",
                 cb[0].size,
                 cb[0].size * UPDATE_TIME,
                 cb[0].count * UPDATE_TIME
          );

     CircularBuffer cbgroup[] = {cb[0], cb[1], cb[2], cb[3]};
     timePlot(cbgroup, "core 0", "C");
     /* draw(cb[1], "core 1", "C", mode); */
     /* draw(cb[2], "core 2", "C", mode); */
     /* draw(cb[3], "core 3", "C", mode); */
     for (int i = 0; i < number_of_threads; i ++) pthread_mutex_unlock(&cb[i].mutex);
}


int main(int, char**)
{
     glfwSetErrorCallback(glfw_error_callback);
     if (!glfwInit())
          return 1;

     // window
     GLFWwindow* window = glfwCreateWindow(XVIEW, YVIEW, "heat", nullptr, nullptr);
     if (window == nullptr)
          return 1;
     glfwMakeContextCurrent(window);
     glfwSwapInterval(1); // Enable vsync

     // context
     IMGUI_CHECKVERSION();
     ImGui::CreateContext();
     ImPlot::CreateContext();
     
     ImGuiIO& io = ImGui::GetIO(); (void)io;
     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
     io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

     ImGui::StyleColorsClassic();
     
     ImGui_ImplGlfw_InitForOpenGL(window, true);
     ImGui_ImplOpenGL2_Init();

     static const int number_of_threads = 4;
     static float font_scale = 1.5;
     static bool quit = 0;
     static bool mode = 0;
     static bool reset = 0;
     static bool show_main = true;

     CircularBuffer cirbu[number_of_threads];
     initBuffer(&cirbu[0], "Core 0");
     initBuffer(&cirbu[1], "Core 1");
     initBuffer(&cirbu[2], "Core 2");
     initBuffer(&cirbu[3], "Core 3");

     pthread_t producerThread[number_of_threads];     

     for (int i = 0; i < number_of_threads; i ++)
     {
          if (pthread_create(&producerThread[i], NULL, temperatureProducer, (void *)&cirbu[i]) != 0)
          {
               perror("failed to create thread\n");
               return 1;
          }
     }

     ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

     // Main loop
     glfw_error_callback(0, "main loop\n");
     while (!glfwWindowShouldClose(window))
     {
          if (ImGui::IsKeyPressed(ImGuiKey_Q)) quit = 1;
          if (ImGui::IsKeyPressed(ImGuiKey_M)) mode = !mode;
          if (ImGui::IsKeyPressed(ImGuiKey_R)) reset = 1;

          glfwPollEvents();

          // Start the Dear ImGui frame
          ImGui_ImplOpenGL2_NewFrame();
          ImGui_ImplGlfw_NewFrame();
          ImGui::NewFrame();

          io.FontGlobalScale = font_scale;

          const ImGuiViewport* viewport = ImGui::GetMainViewport();
          ImGui::SetNextWindowPos(viewport->WorkPos);
          ImGui::SetNextWindowSize(viewport->WorkSize);
          screen_width = viewport->WorkSize.x;
          screen_height = viewport->WorkSize.y;

          ImGui::Begin("heat", &show_main, ImGuiWindowFlags_NoTitleBar);

          presentation(cirbu, number_of_threads, mode);

          ImGui::End();


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

          glfwSetWindowShouldClose(window, quit);
          if (reset)
          {
               reset = 0;
               for (int i = 0; i < number_of_threads; i ++)
                    resetBuffer(&cirbu[i]);
          }
     }

     for (int i = 0; i < number_of_threads; i ++)
     {
          //pthread_join(producerThread[i], NULL);
          printf("destroy and free buffer %d\n", i);
          pthread_mutex_destroy(&cirbu[i].mutex);
          freeBuffer(&cirbu[i]);
     }

     ImGui_ImplOpenGL2_Shutdown();
     ImGui_ImplGlfw_Shutdown();

     ImPlot::DestroyContext();
     ImGui::DestroyContext();

     glfwDestroyWindow(window);
     glfwTerminate();

     return 0;
}
