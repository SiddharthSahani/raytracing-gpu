
#include "src/cl_utils.h"
#include "src/camera.h"
#include "src/test_scenes.h"
#include <raylib/raylib.h>
#include <chrono>


#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define IMAGE_WIDTH (1280/2)
#define IMAGE_HEIGHT (720/2)


void init_render(
    const cl::Context& context, const cl::CommandQueue& queue, const cl::Program& program, cl::Kernel& kernel,
    cl::Buffer& ray_dirs_d, cl::Buffer& pixels_d, std::vector<rt::clScene>& scenes
) {
    auto start = std::chrono::high_resolution_clock::now();
    {
        rt::Camera camera(60.0f, {IMAGE_WIDTH, IMAGE_HEIGHT});
        camera.calculate_ray_directions({0, 0, 6}, {0, 0, -1});
        std::vector<cl_float3> ray_dirs_h = camera.get_ray_directions();
        cl_float3 camera_position = {0, 0, 6, 0};

        ray_dirs_d = cl::Buffer(context, CL_MEM_READ_ONLY, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(cl_float3));
        pixels_d = cl::Buffer(context, CL_MEM_WRITE_ONLY, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(cl_float4));

        queue.enqueueWriteBuffer(ray_dirs_d, true, 0, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(cl_float3), ray_dirs_h.data());

        scenes = {
            create_scene_1(),
            create_scene_2(),
            create_scene_3()
        };

        kernel = cl::Kernel(program, "renderScene");
        kernel.setArg(0, sizeof(cl_float3), &camera_position);
        kernel.setArg(1, ray_dirs_d);
        // kernel.setArg(2, sizeof(rt::clScene), &scene);
        kernel.setArg(3, pixels_d);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto tt_ns = (stop - start).count();
    printf("init_render took %f secs (%f ms)\n", tt_ns / 1e9, tt_ns / 1e6);
}


void render_test_scene(
    const cl::CommandQueue& queue, cl::Kernel& kernel,
    const rt::clScene& scene,
    const cl::Buffer& pixels_d, std::vector<glm::vec4>& pixels_h
) {
    auto start = std::chrono::high_resolution_clock::now();
    {
        kernel.setArg(2, sizeof(rt::clScene), &scene);

        queue.enqueueNDRangeKernel(
            kernel,
            cl::NullRange,
            cl::NDRange(IMAGE_WIDTH * IMAGE_HEIGHT),
            cl::NullRange
        );
        queue.finish();

        queue.enqueueReadBuffer(pixels_d, true, 0, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(glm::vec4), pixels_h.data());
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto tt_ns = (stop - start).count();
    printf("render_test_scene took %f secs (%f ms)\n", tt_ns / 1e9, tt_ns / 1e6);
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
    cl::Kernel kernel;
    cl::Buffer ray_dirs_d, pixels_d;
    std::vector<rt::clScene> scenes;
    init_render(context, queue, program, kernel, ray_dirs_d, pixels_d, scenes);

    int cur_scene_idx = 0;
    render_test_scene(queue, kernel, scenes[cur_scene_idx], pixels_d, out);

    Image image = GenImageGradientRadial(IMAGE_WIDTH, IMAGE_HEIGHT, 0.1f, RAYWHITE, BLACK);
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R32G32B32A32);
    Texture texture = LoadTextureFromImage(image);
    UnloadImage(image);

    UpdateTexture(texture, out.data());

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_LEFT)) {
            cur_scene_idx--;
            if (cur_scene_idx < 0) { cur_scene_idx = scenes.size()-1; }
            render_test_scene(queue, kernel, scenes[cur_scene_idx], pixels_d, out);
            UpdateTexture(texture, out.data());
        }
        if (IsKeyPressed(KEY_RIGHT)) {
            cur_scene_idx++;
            if (cur_scene_idx >= scenes.size()) { cur_scene_idx = 0; }
            render_test_scene(queue, kernel, scenes[cur_scene_idx], pixels_d, out);
            UpdateTexture(texture, out.data());
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTextureEx(texture, {0, 0}, 0.0f, (float) WINDOW_WIDTH/IMAGE_WIDTH, WHITE);

        // DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();
}
