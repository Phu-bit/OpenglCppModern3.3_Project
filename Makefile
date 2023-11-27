# Define the compiler
CXX = g++
# CXX = clang++ # or any other compiler you wish to use

# Define the executable name
EXE = OpenGL_Tester

# Directories containing all the headers and libraries
INCLUDE_DIRS = -ILibraries/include -ILibraries/include/glad -ILibraries/include/GLFW \
               -ILibraries/include/glm -ILibraries/include/ImGui -ILibraries/include/json \
               -ILibraries/include/KHR -ILibraries/include/stb
LIBRARY_DIRS = -LLibraries/lib

# Define any compile-time flags
CXXFLAGS = -std=c++17 -g -Wall -Wformat $(INCLUDE_DIRS) -w
LIBS = -lglfw3 -lopengl32 -lgdi32 -limm32

CLEAN=rm -f *.exe *.o *.a

# Define the source files
SOURCES = main.cpp \
          glad.c \
          imgui.cpp \
          imgui_demo.cpp \
          imgui_draw.cpp \
          imgui_widgets.cpp \
          imgui_tables.cpp \
          imgui_impl_glfw.cpp \
          imgui_impl_opengl3.cpp \
          Camera.cpp \
          EBO.cpp \
          Mesh.cpp \
          Model.cpp \
          shaderClass.cpp \
          Texture.cpp \
          VAO.cpp \
          VBO.cpp \
          stp.cpp # replace with .c if that's your file extension

# Prefix ImGui sources with their directory
IMGUI_DIR = Libraries/ImGui
IMGUI_SOURCES = $(addprefix $(IMGUI_DIR)/,$(filter imgui%,$(SOURCES)))
# Exclude ImGui sources from the main SOURCES variable
SOURCES := $(filter-out imgui%,$(SOURCES))

# Define the object files (both regular and ImGui-specific)
OBJS = $(SOURCES:.cpp=.o) $(IMGUI_SOURCES:.cpp=.o) $(SOURCES:.c=.o)

# OS specific part
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
    LIBS += -ldl -lGL -lX11 -lpthread -lXrandr -lXi
endif

ifeq ($(UNAME_S), Darwin)
    LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    LIBS += -L/usr/local/lib -L/opt/local/lib
    CXXFLAGS += -I/usr/local/include -I/opt/local/include
endif

ifeq ($(OS), Windows_NT)
    LIBS += -limm32
endif

# Define the rules for building the executable
all: $(EXE)
	@echo Build complete for $(UNAME_S)

# Main executable build rule
$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBRARY_DIRS) $(LIBS)

# Rule for building object files from cpp files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Rule for building object files from c files
%.o: %.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Rule for building ImGui object files from cpp files
$(IMGUI_DIR)/%.o: $(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean up build artifacts
clean:
	$(CLEAN)

.PHONY: all clean
