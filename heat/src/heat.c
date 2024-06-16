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
#include "k3key.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#define CODEBY "Kaloyan Krastev"
#define VERSION "heat v5.3.0"
#define XVIEW 999
#define YVIEW 666
#define DEBUG true
#define HISTO_SIZE 6
#define HIGH_TEMPERATURE 77
#define LOW_TEMPERATURE 66
#define NUMBER_OF_THREADS 4
#define SHOW_SIZE 4
#define SHOW_MAIN 0
#define SHOW_HELP 1
#define SHOW_ABOUT 2
#define SHOW_DEBUG 3


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
     int c = (1 << 8) - 1;
     int d = (1 << 7) - 1;
     int q = (1 << 6) - 1;
     int t = d;
     if (strcmp(colorName, "red") == 0) return IM_COL32(c, 0, 0, c);
     else if (strcmp(colorName, "green")   == 0) return IM_COL32(0, c, 0, t);
     else if (strcmp(colorName, "blue")    == 0) return IM_COL32(0, 0, c, t);
     else if (strcmp(colorName, "yellow")  == 0) return IM_COL32(c, c, 0, t);
     else if (strcmp(colorName, "cyan")    == 0) return IM_COL32(0, c, c, t);
     else if (strcmp(colorName, "magenta") == 0) return IM_COL32(c, 0, c, t);
     else if (strcmp(colorName, "black")   == 0) return IM_COL32(0, 0, 0, t);
     else if (strcmp(colorName, "gray")    == 0) return IM_COL32(d, d, d, t);
     else if (strcmp(colorName, "orange")  == 0) return IM_COL32(c, d, q, t);
     else return IM_COL32(c, c, c, t);
}


void setStyle(const char * mystyle) {

     if (strcmp(mystyle, "dark") == 0) ImGui::StyleColorsDark();
     else if (strcmp(mystyle, "light") == 0) ImGui::StyleColorsLight();
     else ImGui::StyleColorsClassic();
     
     ImPlotStyle& style              = ImPlot::GetStyle();

     ImVec4* colors                  = style.Colors;
     colors[ImPlotCol_Line]          = IMPLOT_AUTO_COL;
     colors[ImPlotCol_Fill]          = IMPLOT_AUTO_COL;
     colors[ImPlotCol_MarkerOutline] = IMPLOT_AUTO_COL;
     colors[ImPlotCol_MarkerFill]    = IMPLOT_AUTO_COL;
     /* colors[ImPlotCol_ErrorBar]      = ImVec4(0.00f, 0.00f, 0.00f, 1.00f); */
     /* colors[ImPlotCol_FrameBg]       = ImVec4(1.00f, 1.00f, 1.00f, 1.00f); */
     /* colors[ImPlotCol_PlotBg]        = ImVec4(0.92f, 0.92f, 0.95f, 1.00f); */
     /* colors[ImPlotCol_PlotBorder]    = ImVec4(0.00f, 0.00f, 0.00f, 0.00f); */
     colors[ImPlotCol_LegendBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
     /* colors[ImPlotCol_LegendBorder]  = ImVec4(0.80f, 0.81f, 0.85f, 1.00f); */
     /* colors[ImPlotCol_LegendText]    = ImVec4(0.00f, 0.00f, 0.00f, 1.00f); */
     /* colors[ImPlotCol_TitleText]     = ImVec4(0.00f, 0.00f, 0.00f, 1.00f); */
     /* colors[ImPlotCol_InlayText]     = ImVec4(0.00f, 0.00f, 0.00f, 1.00f); */
     /* colors[ImPlotCol_AxisText]      = ImVec4(0.00f, 0.00f, 0.00f, 1.00f); */
     /* colors[ImPlotCol_AxisGrid]      = ImVec4(1.00f, 1.00f, 1.00f, 1.00f); */
     /* colors[ImPlotCol_AxisBgHovered]   = ImVec4(0.92f, 0.92f, 0.95f, 1.00f); */
     /* colors[ImPlotCol_AxisBgActive]    = ImVec4(0.92f, 0.92f, 0.95f, 0.75f); */
     /* colors[ImPlotCol_Selection]     = ImVec4(1.00f, 0.65f, 0.00f, 1.00f); */
     /* colors[ImPlotCol_Crosshairs]    = ImVec4(0.23f, 0.10f, 0.64f, 0.50f); */

     style.LineWeight       = 3;
     style.Marker           = ImPlotMarker_None;
     style.MarkerSize       = 4;
     style.MarkerWeight     = 1;
     style.FillAlpha        = 0.6f;
     style.ErrorBarSize     = 5;
     style.ErrorBarWeight   = 1.5f;
     style.DigitalBitHeight = 8;
     style.DigitalBitGap    = 4;
     style.PlotBorderSize   = 0;
     style.MinorAlpha       = 1.0f;
     style.MajorTickLen     = ImVec2(0, 0);
     style.MinorTickLen     = ImVec2(0, 0);
     style.MajorTickSize    = ImVec2(0, 0);
     style.MinorTickSize    = ImVec2(0, 0);
     style.MajorGridSize    = ImVec2(1.2f, 1.2f);
     style.MinorGridSize    = ImVec2(1.2f, 1.2f);
     style.PlotPadding      = ImVec2(12, 12);
     style.LabelPadding     = ImVec2(5, 5);
     style.LegendPadding    = ImVec2(5, 5);
     style.MousePosPadding  = ImVec2(5, 5);
     style.PlotMinSize      = ImVec2(300, 225);
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

void getLimit(CircularBuffer cb[], float * minp, float * maxp)
{
     float minval = 1e8;
     float maxval = -1e8;
     for (int i = 0; i < NUMBER_OF_THREADS; i ++)
     {
          minval = minval < cb[i].min ? minval : cb[i].min;
          maxval = maxval > cb[i].max ? maxval : cb[i].max;
     }

     *minp = minval;
     *maxp = maxval;

     if (maxval > HIGH_TEMPERATURE) setStyle("light");
     if (maxval < LOW_TEMPERATURE) setStyle("dark");
}


ImU32 cool[] = {Couleur("magenta"), Couleur("cyan"), Couleur("yellow"), Couleur("orange")};

static void spacePlot(CircularBuffer cb[], const char* title = "", const float plot_size = 1)
{
     float ymin, ymax;
     getLimit(cb, &ymin, &ymax);

     ImPlot::SetNextAxesLimits(ymin, ymax, 0, 0.66, ImGuiCond_Always);
     //ImPlot::SetNextAxisToFit(ImAxis_X1);
     if (ImPlot::BeginPlot("histogram", ImVec2(-1, plot_size)))
     {
          //ImPlot::SetupAxisLimits(ImAxis_X1, ymin, ymax, ImGuiCond_Always);
          //ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImGuiCond_Always);
          //ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
          //ImPlot::SetupAxis(ImAxis_X1, "", ImPlotAxisFlags_AutoFit);
          ImPlot::SetupAxis(ImAxis_Y1, "", ImPlotAxisFlags_AutoFit);// | ImPlotAxisFlags_AuxDefault);
          ImPlot::SetupAxis(ImAxis_Y1, "", ImPlotAxisFlags_NoTickLabels);
          for (int i = 0; i < NUMBER_OF_THREADS; i ++)
          {
               const float * data = cb[i].data;
               const size_t size = cb[i].size;
               
               ImPlot::PushStyleColor(ImPlotCol_Fill, cool[i]);
               ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
               ImPlot::PlotHistogram("", data, size, HISTO_SIZE, 1.0, ImPlotRange(), ImPlotHistogramFlags_Density);
          }

          ImPlot::EndPlot();
     }
}


static void timePlot(CircularBuffer cb[], const char* title = "", const float plot_size = 1)
{
     float ymin, ymax;
     getLimit(cb, &ymin, &ymax);

     ImPlot::SetNextAxesLimits(-1 * (float)cb[0].size, 0.0f, ymin, ymax, ImGuiCond_Always);
     if (ImPlot::BeginPlot(title, ImVec2(-1, plot_size)))
     {
          ImPlot::SetupAxis(ImAxis_Y1, "", ImPlotAxisFlags_NoTickLabels);
          ImPlot::SetupAxis(ImAxis_Y2, "", ImPlotAxisFlags_AuxDefault | ImPlotAxisFlags_AutoFit);
          ImPlot::SetupAxisLimits(ImAxis_Y2, ymin, ymax, ImGuiCond_Always);
          //ImPlot::SetupAxis(ImAxis_X1, "", ImPlotAxisFlags_AuxDefault | ImPlotAxisFlags_AutoFit);
          for (int i = 0; i < NUMBER_OF_THREADS; i ++)
          {
               const char * label = formatString("core %d %.1f %.1f %.1f", i, ymin, cb[i].last, ymax);
               const float * data = cb[i].data;
               const size_t size = cb[i].size;
               
               float time[size];
               for (size_t j = 0; j < size; j ++)
               {
                    time[j] = -1 * (float)cb[i].size + (float)j;
               }               

               ImPlot::PushStyleColor(ImPlotCol_Line, cool[i]);
               ImPlot::PlotLine(label, time, data, size);

               if (label) free((void *)label);
          }

          ImPlot::EndPlot();
     }
}

/* static void draw(const CircularBuffer objbuf, const char* title, const char* siunit, bool mode = 0) */
/* { */
/*      if (mode) spacePlot(objbuf, title, siunit); */
/*      else timePlot(objbuf, objbuf, objbuf, objbuf, title, siunit); */
/* } */


static void presentation(CircularBuffer cb[], const bool mode, size_t * size, size_t * count)
{
     for (int i = 0; i < NUMBER_OF_THREADS; i ++) pthread_mutex_lock(&cb[i].mutex);

     *size = cb[0].size;
     *count = cb[0].count;

     ImVec2 full_size_vec = ImGui::GetContentRegionAvail();
     float full_size = full_size_vec.y; 
     CircularBuffer cbgroup[] = {cb[0], cb[1], cb[2], cb[3]};
     if (mode)
     {
          spacePlot(cbgroup, "temperature/°C", full_size / 2);
          timePlot(cbgroup, "temperature/°C", full_size / 2);
     } else {
          timePlot(cbgroup, "temperature/°C", full_size / 2);
          spacePlot(cbgroup, "temperature/°C", full_size / 2);
     }
     for (int i = 0; i < NUMBER_OF_THREADS; i ++) pthread_mutex_unlock(&cb[i].mutex);
}


int main(int, char**)
{
     glfwSetErrorCallback(glfw_error_callback);
     if (!glfwInit()) return 1;

     // window
     GLFWwindow* window = glfwCreateWindow(XVIEW, YVIEW, "heat", nullptr, nullptr);
     if (window == nullptr) return 1;
     glfwMakeContextCurrent(window);
     glfwSwapInterval(1); // Enable vsync

     // context
     IMGUI_CHECKVERSION();
     ImGui::CreateContext();
     ImPlot::CreateContext();
     
     ImGuiIO& io = ImGui::GetIO(); (void)io;
     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
     io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

     ImGui_ImplGlfw_InitForOpenGL(window, true);
     ImGui_ImplOpenGL2_Init();

     static float font_scale = 1.5;
     static bool quit = 0;
     static bool mode = 0;
     static bool reset = 0;
     static k3key cle = create_k3key(SHOW_SIZE);
     cle.on(&cle, SHOW_MAIN);
     cle.dump(&cle);

     CircularBuffer cirbu[NUMBER_OF_THREADS];
     initBuffer(&cirbu[0], "Core 0");
     initBuffer(&cirbu[1], "Core 1");
     initBuffer(&cirbu[2], "Core 2");
     initBuffer(&cirbu[3], "Core 3");

     pthread_t producerThread[NUMBER_OF_THREADS];     

     for (int i = 0; i < NUMBER_OF_THREADS; i ++)
     {
          sleep(0.1);
          if (pthread_create(&producerThread[i], NULL, temperatureProducer, (void *)&cirbu[i]) != 0)
          {
               perror("failed to create thread\n");
               return 1;
          }
     }

     ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

     // Main loop
     glfw_error_callback(0, "main loop\n");
     setStyle("classic");
     
     while (!glfwWindowShouldClose(window))
     {
          if (ImGui::IsKeyPressed(ImGuiKey_A)) cle.flip(&cle, SHOW_ABOUT);
          if (ImGui::IsKeyPressed(ImGuiKey_D)) cle.flip(&cle, SHOW_DEBUG);
          if (ImGui::IsKeyPressed(ImGuiKey_H)) cle.flip(&cle, SHOW_HELP);
          if (ImGui::IsKeyPressed(ImGuiKey_Escape)) cle.flip(&cle, SHOW_HELP);
          if (ImGui::IsKeyPressed(ImGuiKey_M)) mode = !mode;
          if (ImGui::IsKeyPressed(ImGuiKey_Q)) quit = 1;
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

          static size_t buf_size, buf_count;
          if (ImGui::Begin("heat", cle.get(&cle, SHOW_MAIN), ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize))
          {
               presentation(cirbu, mode, &buf_size, &buf_count);
               ImGui::End();
          }

          if (*cle.get(&cle, SHOW_DEBUG))
               if (ImGui::Begin("debug", cle.get(&cle, SHOW_DEBUG), ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar))
               {
                    ImGui::SeparatorText("debug");
                    ImGui::Text("%.0f%% %zus", (float)buf_size / BUFFER_SIZE * 100, buf_count * (size_t)UPDATE_TIME);
                    ImGui::End();
               }

          if (*cle.get(&cle, SHOW_ABOUT))
               if (ImGui::Begin("about", cle.get(&cle, SHOW_ABOUT), ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar))
               {
                    ImGui::SeparatorText("about");
                    ImGui::Text("%s", VERSION);
                    ImGui::SeparatorText("code");
                    ImGui::Text("%s", CODEBY);
                    ImGui::SeparatorText("powered by");
                    ImGui::Text("ImPlot, ImGui, GLFW, OpenGL, X11");
                    ImGui::End();
               }

          if (*cle.get(&cle, SHOW_HELP))
               if (ImGui::Begin("help", cle.get(&cle, SHOW_HELP), ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar))
               {
                    ImGui::SeparatorText("help");
                    ImGui::Separator();
                    ImGui::SeparatorText("keyboard control");
                    ImGui::Text("[a] toggle about window");
                    ImGui::Text("[d] toggle debug window");
                    ImGui::Text("[h] toggle help window");
                    ImGui::Text("[m] toggle visualisation mode");
                    ImGui::Text("[q] quit %s", VERSION);
                    ImGui::Text("[r] reset data");
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

          glfwSetWindowShouldClose(window, quit);
          if (reset)
          {
               reset = 0;
               for (int i = 0; i < NUMBER_OF_THREADS; i ++)
                    resetBuffer(&cirbu[i]);
          }
     }

     for (int i = 0; i < NUMBER_OF_THREADS; i ++)
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
