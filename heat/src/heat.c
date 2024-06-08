#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
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
#define YVIEW 666
#define DEBUG true
#define VERSION "0.0.3"
#define WIN_ABOUT 0
#define WIN_DEBUG 1
#define WIN_CONTROL 2
#define HISTO_SIZE 1e1


int screen_width;
int screen_height;

static void glfw_error_callback(int error, const char* description)
{
     fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static ImVec2 plain(void)
{
     int* vjhbiyg = &screen_width;
     int* hjjhvgf = &screen_height;
     ImVec2 p(*vjhbiyg - 16, *hjjhvgf / 6);
     return p;
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

static void spacePlot(const CircularBuffer cb, const char* title = "", const char* siunit = "")
{
     float hmin = 0;
     float hmax = 0;
     float hsize = 0;
     float * hist = histogram(&cb, HISTO_SIZE, &hmin, &hmax, &hsize);
     const char * overlay = formatString("%s %.0f", title, hsize);
     ImGui::PlotHistogram("", hist, hsize, 0, overlay, hmin, hmax, plain());
     if (hist) free((void *)hist);
}

static void timePlot(const CircularBuffer cb, const char* title = "", const char* siunit = "")
{
     const char * lay = formatString("%6.1f [%6.1f ; %6.1f]", cb.last, cb.min, cb.max);
     const char * overlay = formatString("%s %s %s", title, lay, siunit);
     ImGui::PlotLines("", cb.data, cb.size, 0, overlay, cb.min, cb.max, plain());
     if (overlay != NULL) free((void *)overlay);
     if (lay != NULL) free((void *)lay);
}

static void draw(const CircularBuffer objbuf, const char* title, const char* siunit, bool mode = 0)
{
     if (mode) spacePlot(objbuf, title, siunit);
     else timePlot(objbuf, title, siunit);
}


static void presentation(CircularBuffer cb[], int number_of_threads, bool mode = 0)
{
     for (int i = 0; i < number_of_threads; i ++) pthread_mutex_lock(&cb[i].mutex);
     ImGui::Text("%zu %.1fs %.1fs",
                 cb[0].size,
                 cb[0].size * UPDATE_TIME,
                 cb[0].count * UPDATE_TIME
          );

     draw(cb[0], "core 0", "C", mode);
     draw(cb[1], "core 1", "C", mode);
     draw(cb[2], "core 2", "C", mode);
     draw(cb[3], "core 3", "C", mode);
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
     ImGuiIO& io = ImGui::GetIO(); (void)io;
     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
     io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

     ImGui::StyleColorsDark();

     ImGui_ImplGlfw_InitForOpenGL(window, true);
     ImGui_ImplOpenGL2_Init();

     static const int number_of_threads = 4;
     static float font_scale = 2;
     static bool quit = 0;
     static bool mode = 0;
     static bool reset = 0;

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

          ImGui::Begin("heat");

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
     ImGui::DestroyContext();

     glfwDestroyWindow(window);
     glfwTerminate();

     return 0;
}
