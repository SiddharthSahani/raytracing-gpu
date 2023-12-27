
CXXFLAGS =
DEFINES  = -DCL_HPP_TARGET_OPENCL_VERSION=120 -DCL_HPP_MINIMUM_OPENCL_VERSION=120
INCLUDES = -I . -I external/includes/
LDFLAGS  = -L external/libs -lopencl -lraylib -lopengl32 -lgdi32 -lwinmm

HEADERS = $(wildcard src/*.h)
SOURCES = $(wildcard src/*.cpp)

TARGET = main.exe


$(TARGET): clean main.cpp $(SOURCES) $(HEADERS)
	g++ -o $(TARGET) main.cpp $(SOURCES) $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(LDFLAGS)


clean:
	rm -rf $(TARGET)
