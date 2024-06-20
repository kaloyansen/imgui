#ifndef HEAT_H
#define HEAT_H
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GLFW/glfw3.h>
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#include "implot.h"
#include "implot_internal.h"
#include "k3buf.h"
#include "k3key.h"

#define CODEBY "Kaloyan Krastev"
#define VERSION "heat v2.3.0"
#define XVIEW 999
#define YVIEW 666
#define DEBUG true
#define HISTO_SIZE 6
#define HIGH_TEMPERATURE 77
#define LOW_TEMPERATURE 66
#define NUMBER_OF_THREADS 6

enum SHOW_{SHOW_HELP, SHOW_ABOUT, SHOW_DEBUG, SHOW_SIZE};

static void glfw_error_callback(int, const char *);
ImU32 couleur(const char *);
void setStyle(const char *);
int nextStyle();
const char* formatString(const char *, ...);
static void spacePlot(k3buf *);
static void timePlot(k3buf *);
static void talk(k3buf *, const bool, size_t *, size_t *);

#endif
