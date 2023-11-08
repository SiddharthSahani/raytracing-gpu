
CXXFLAGS =
INCLUDES = -I . -I external/includes/
LDFLAGS  = -L external/libs -lopencl -lraylib -lopengl32 -lgdi32 -lwinmm

HEADERS = $(wildcard src/*.h)
SOURCES = $(wildcard src/*.cpp)

TARGET = main.exe


$(TARGET): main.cpp $(SOURCES) $(HEADERS)
	g++ -o $(TARGET) main.cpp $(SOURCES) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS)
