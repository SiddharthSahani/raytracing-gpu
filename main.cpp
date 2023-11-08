
#include "src/camera.h"
#include <CL/opencl.hpp>
#include <raylib/raylib.h>
#include <vector>
#include <fstream>


#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define IMAGE_WIDTH (1280/2)
#define IMAGE_HEIGHT (720/2)


bool get_device(cl::Device& selected_device) {
    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);

    std::vector<cl::Device> all_devices;
    for (const auto& plat : all_platforms) {
        std::vector<cl::Device> temp;
        plat.getDevices(CL_DEVICE_TYPE_ALL, &temp);
        all_devices.insert(all_devices.end(), temp.begin(), temp.end());
    }

    if (all_devices.size() == 0) {
        printf("No OpenCl compactible device found\n");
        return false;
    }
    if (all_devices.size() == 1) {
        selected_device = all_devices[0];
        return true;
    }

    printf("Multiple devices found, choose one:\n");
    for (int i = 0; i < all_devices.size(); i++) {
        const auto& device = all_devices[i];
        printf("%d) %s [%d CU]\n", i+1, device.getInfo<CL_DEVICE_NAME>().c_str(), device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>());
    }

    int selected_device_idx;
    printf("Enter selection: ");
    scanf("%d", &selected_device_idx);

    selected_device = all_devices[selected_device_idx-1];
    return true;
}


void print_device_info(cl::Device& device) {
    auto cu_count = device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
    auto clock_freq = device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>();
    auto dev_name = device.getInfo<CL_DEVICE_NAME>();
    auto version = device.getInfo<CL_DEVICE_VERSION>();
    auto cl_version = device.getInfo<CL_DEVICE_OPENCL_C_VERSION>();

    printf("----- Selected Device Info -----\n");
    printf("  Name: %s\n", dev_name.c_str());
    printf("  Version: %s\n", version.c_str());
    printf("  OpenCl C Version: %s\n", cl_version.c_str());
    printf("  Compute Units: %d\n", cu_count);
    printf("  Clock Frequency: %d\n", clock_freq);
}


std::string read_file(const char* filepath) {
    std::ifstream file(filepath, std::ios::in | std::ios::ate);

    int file_size = file.tellg();
    std::string file_content(file_size, '\0');

    file.seekg(0);
    file.read(&file_content[0], file_size);
    return file_content;
}


bool create_opencl_objects(const cl::Device& device, cl::Context& context, cl::CommandQueue& queue, cl::Program& program) {
    context = cl::Context(device);
    queue = cl::CommandQueue(context, device);

    std::string program_string = read_file("kernels/renderer.cl");
    program = cl::Program(program_string);

    if (program.build()) {
        printf("Unable to build OpenCl program\n");
        std::string build_log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
        printf("Build Log:\n%s\n", build_log.c_str());
        return false;
    }

    return true;
}


void render_test_scene(const cl::Context& context, const cl::CommandQueue& queue, cl::Program& program, std::vector<glm::vec4>& pixels_h) {
    rt::Camera camera(60.0f, {IMAGE_WIDTH, IMAGE_HEIGHT});
    camera.set_params({0, 0, 6}, {0, 0, -1});
    std::vector<rt::Ray> rays_h = camera.get_rays();

    cl::Buffer rays_d(context, CL_MEM_READ_ONLY, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(rt::Ray));
    cl::Buffer pixels_d(context, CL_MEM_WRITE_ONLY, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(glm::vec4));

    queue.enqueueWriteBuffer(rays_d, true, 0, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(rt::Ray), rays_h.data());

    cl::Kernel kernel(program, "renderScene");
    kernel.setArg(0, rays_d);
    kernel.setArg(1, pixels_d);

    queue.enqueueNDRangeKernel(
        kernel,
        cl::NullRange,
        cl::NDRange(IMAGE_WIDTH * IMAGE_HEIGHT),
        cl::NullRange
    );
    queue.finish();

    queue.enqueueReadBuffer(pixels_d, true, 0, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(glm::vec4), pixels_h.data());
}


int main() {
    cl::Device device;
    printf("OpenCl device selected completed: %d\n", get_device(device));
    print_device_info(device);

    cl::Context context;
    cl::CommandQueue queue;
    cl::Program program;
    if (!create_opencl_objects(device, context, queue, program)) {
        return -1;
    }

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(1280, 720, "OpenCl with raylib");
    SetTargetFPS(30);
    
    std::vector<glm::vec4> out(IMAGE_WIDTH*IMAGE_HEIGHT, glm::vec4(0, 1, 0, 1));
    render_test_scene(context, queue, program, out);

    Image image = GenImageGradientRadial(IMAGE_WIDTH, IMAGE_HEIGHT, 0.1f, RAYWHITE, BLACK);
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R32G32B32A32);
    Texture texture = LoadTextureFromImage(image);
    UnloadImage(image);

    UpdateTexture(texture, out.data());

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTextureEx(texture, {0, 0}, 0.0f, (float) WINDOW_WIDTH/IMAGE_WIDTH, WHITE);

        // DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();
}
