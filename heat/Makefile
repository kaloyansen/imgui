#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
# You will need GLFW (http://www.glfw.org):
# debian linux:
#   apt-get install libglfw-dev
# manjaro linux:
#   pamac install glfw-x11
# Mac OS X:
#   brew install glfw
# MSYS2:
#   pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw
#

#CXX = g++
#CXX = clang++

BINARY = heat

IMGUI_DIR = ..
IMPLOT_DIR = ../../implot
SRC_DIR = $(IMGUI_DIR)/$(BINARY)/src
INC_DIR = $(IMGUI_DIR)/$(BINARY)/include

SOURCE = $(IMGUI_DIR)/imgui.cpp
SOURCE += $(IMGUI_DIR)/imgui_draw.cpp
SOURCE += $(IMGUI_DIR)/imgui_tables.cpp
SOURCE += $(IMGUI_DIR)/imgui_widgets.cpp
SOURCE += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp
SOURCE += $(IMGUI_DIR)/backends/imgui_impl_opengl2.cpp
# SOURCE += $(IMGUI_DIR)/imgui_demo.cpp
SOURCE += ${IMPLOT_DIR}/implot.cpp
SOURCE += ${IMPLOT_DIR}/implot_items.cpp
SOURCE += $(SRC_DIR)/k3buf.c
SOURCE += $(SRC_DIR)/k3key.c
SOURCE += $(SRC_DIR)/$(BINARY).c

OBJ = $(addsuffix .o, $(basename $(notdir $(SOURCE))))
UNAME_S := $(shell uname -s)

INCFLAG = -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I$(INC_DIR) -I$(IMPLOT_DIR)
CXXFLAG = $(INCFLAG) -g -Wall -Wformat
#CXXFLAG += -std=c++11
LIBS =

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "linux"
	LIBS += -lGL `pkg-config --static --libs glfw3`

	CXXFLAG += `pkg-config --cflags glfw3`
	CFLAG = $(CXXFLAG)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	LIBS += -L/usr/local/lib -L/opt/local/lib -L/opt/homebrew/lib
	LIBS += -lglfw

	CXXFLAG += -I/usr/local/include -I/opt/local/include -I/opt/homebrew/include
	CFLAG = $(CXXFLAG)
endif

ifeq ($(OS), Windows_NT)
	ECHO_MESSAGE = "MinGW"
	LIBS += -lglfw3 -lgdi32 -lopengl32 -limm32

	CXXFLAG += `pkg-config --cflags glfw3`
	CFLAG = $(CXXFLAG)
endif

MESSAGE = $(BINARY) built on $(ECHO_MESSAGE)

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

%.o:	%.cpp
	$(CXX) $(CXXFLAG) -c -o $@ $<

%.o:	$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAG) -c -o $@ $<

%.o:	$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAG) -c -o $@ $<

%.o:	$(IMPLOT_DIR)/%.cpp
	$(CXX) $(CXXFLAG) -c -o $@ $<

%.o:	$(SRC_DIR)/%.c
	$(CXX) $(CFLAG) -c -o $@ $<

all:	$(BINARY)
	@echo $(MESSAGE)

$(BINARY): $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAG) $(LIBS)

clobber:
	rm -f $(BINARY) $(OBJ)

clean:
	rm -f k3buf.o heat.o k3key.o

