
CXXFLAGS =
DEFINES  = -DCL_HPP_TARGET_OPENCL_VERSION=120 -DCL_HPP_MINIMUM_OPENCL_VERSION=120
INCLUDES = -I . -I external/includes/
LDFLAGS  = -L external/libs -lopencl

HEADERS = $(wildcard src/*.h)


clean:
	rm -rf $(wildcard examples/*.exe)


raylib: clean examples/main_raylib.cpp $(HEADERS)
	g++ -o examples/main_raylib.exe examples/main_raylib.cpp src/clutils.cpp src/raytracer.cpp src/backend/raylib/renderer.cpp src/backend/raylib/camera.cpp $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(LDFLAGS) -lraylib -lgdi32 -lwinmm


nogui: clean examples/main_nogui.cpp $(HEADERS)
	g++ -o examples/main_nogui.exe examples/main_nogui.cpp src/clutils.cpp src/raytracer.cpp -DSTB_IMAGE_WRITE_IMPLEMENTATION $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(LDFLAGS)
