
CXXFLAGS =
DEFINES  = -DCL_HPP_TARGET_OPENCL_VERSION=120 -DCL_HPP_MINIMUM_OPENCL_VERSION=120
INCLUDES = -I . -I external/includes/
LDFLAGS  = -L external/libs -lopencl


COMMON_SOURCES = $(wildcard src/*.cpp)
COMMON_OBJECTS = $(COMMON_SOURCES:.cpp=.o)

BACKEND_RAYLIB_SOURCES = $(wildcard src/backend/raylib/*.cpp)
BACKEND_RAYLIB_OBJECTS = $(BACKEND_RAYLIB_SOURCES:.cpp=.o)


raylib: examples/main_raylib.cpp $(COMMON_OBJECTS) $(BACKEND_RAYLIB_OBJECTS)
	g++ -o examples/main_raylib.exe $^ $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(LDFLAGS) -lraylib -lgdi32 -lopengl32 -lwinmm


nogui: examples/main_nogui.cpp $(COMMON_OBJECTS)
	g++ -o examples/main_nogui.exe $^ $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(LDFLAGS) -lopengl32


%.o: %.cpp
	g++ -o $@ -c $< $(DEFINES) $(CXXFLAGS) $(INCLUDES)


clean:
	rm -rf $(wildcard examples/*.exe)
	rm -rf $(wildcard src/*.o)
	rm -rf $(wildcard src/backend/raylib/*.o)
