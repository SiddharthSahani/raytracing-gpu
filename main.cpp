
#include "src/cl_utils.h"
#include "src/camera.h"
#include "src/test_scenes.h"
#include <raylib/raylib.h>
#include <chrono>


#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define IMAGE_WIDTH (1280/2)
#define IMAGE_HEIGHT (720/2)


namespace rt {

struct RendererConfig {
    cl_uint sample_count;
    cl_uint bounce_limit;
};

}

// NOTE: temporary, to not deal with argument passing stuff
namespace vars {

cl::Device cl_device_obj;
cl::Context cl_context_obj;
cl::CommandQueue cl_queue_obj;
cl::Program cl_program_obj;
cl::Kernel cl_kernel_obj;
cl::Buffer pixels_d;

std::vector<rt::CompiledScene> scenes;
int cur_scene_idx;
rt::RendererConfig config;

}


void recreate_kernel() {
    build_program(vars::cl_program_obj, vars::cl_device_obj);

    glm::vec3 camera_position = {0, 0, 6};
    glm::vec3 camera_direction = {0, 0, -1};
    rt::clCamera camera = rt::create_camera(60.0f, {IMAGE_WIDTH, IMAGE_HEIGHT}, camera_position, camera_direction);

    vars::cl_kernel_obj = cl::Kernel(vars::cl_program_obj, "renderScene");
    vars::cl_kernel_obj.setArg(0, sizeof(rt::clCamera), &camera);
    vars::cl_kernel_obj.setArg(3, vars::pixels_d);
}


void init_render() {
    auto start = std::chrono::high_resolution_clock::now();
    {
        vars::config = rt::RendererConfig{
            .sample_count = 32,
            .bounce_limit = 5
        };

        vars::pixels_d = cl::Buffer(vars::cl_context_obj, CL_MEM_WRITE_ONLY, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(cl_float4));

        vars::scenes = {
            create_scene_1(),
            create_scene_2(),
            create_scene_3(),
            create_scene_4()
        };

        recreate_kernel();
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto tt_ns = (stop - start).count();
    printf("init_render took %f secs (%f ms)\n", tt_ns / 1e9, tt_ns / 1e6);
}


void render_test_scene(glm::vec4* out) {
    auto start = std::chrono::high_resolution_clock::now();
    {
        vars::cl_kernel_obj.setArg(1, sizeof(rt::CompiledScene), &vars::scenes[vars::cur_scene_idx]);
        vars::cl_kernel_obj.setArg(2, sizeof(rt::RendererConfig), &vars::config);

        vars::cl_queue_obj.enqueueNDRangeKernel(
            vars::cl_kernel_obj,
            cl::NullRange,
            cl::NDRange(IMAGE_WIDTH * IMAGE_HEIGHT),
            cl::NullRange
        );
        vars::cl_queue_obj.finish();

        vars::cl_queue_obj.enqueueReadBuffer(vars::pixels_d, true, 0, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(glm::vec4), out);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto tt_ns = (stop - start).count();
    printf("render_test_scene took %f secs (%f ms)\n", tt_ns / 1e9, tt_ns / 1e6);
}


bool check_scene_change() {
    if (IsKeyDown(KEY_C) && IsKeyPressed(KEY_LEFT)) {
        vars::cur_scene_idx--;
        if (vars::cur_scene_idx < 0) { vars::cur_scene_idx = vars::scenes.size() - 1; }
        return true;
    }
    if (IsKeyDown(KEY_C) && IsKeyPressed(KEY_RIGHT)) {
        vars::cur_scene_idx++;
        if (vars::cur_scene_idx == vars::scenes.size()) { vars::cur_scene_idx = 0; }
        return true;
    }

    return false;
}


void change_config() {
    if (IsKeyDown(KEY_S) && IsKeyPressed(KEY_EQUAL)) {
        vars::config.sample_count *= 1.5;
    }
    if (IsKeyDown(KEY_S) && IsKeyPressed(KEY_MINUS)) {
        vars::config.sample_count /= 1.5;
        if (vars::config.sample_count < 1) { vars::config.sample_count = 1; }
    }

    if (IsKeyDown(KEY_B) && IsKeyPressed(KEY_EQUAL)) {
        vars::config.bounce_limit++;
    }
    if (IsKeyDown(KEY_B) && IsKeyPressed(KEY_MINUS)) {
        vars::config.bounce_limit--;
        if (vars::config.bounce_limit < 1) { vars::config.bounce_limit = 1; }
    }
}


int main() {
    printf("OpenCl device selected completed: %d\n", get_device(vars::cl_device_obj));
    print_device_info(vars::cl_device_obj);

    create_opencl_objects(vars::cl_device_obj, vars::cl_context_obj, vars::cl_queue_obj, vars::cl_program_obj);

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenCl with raylib");
    SetTargetFPS(30);
    
    std::vector<glm::vec4> out(IMAGE_WIDTH*IMAGE_HEIGHT, glm::vec4(0, 1, 0, 1));
    init_render();

    int cur_scene_idx = 0;
    render_test_scene(out.data());

    Image image = GenImageGradientRadial(IMAGE_WIDTH, IMAGE_HEIGHT, 0.1f, RAYWHITE, BLACK);
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R32G32B32A32);
    Texture texture = LoadTextureFromImage(image);
    UnloadImage(image);

    UpdateTexture(texture, out.data());

    while (!WindowShouldClose()) {
        if (check_scene_change()) {
            render_test_scene(out.data());
            UpdateTexture(texture, out.data());            
        }

        change_config();
        if (IsKeyPressed(KEY_SPACE)) {
            printf("Config:\n  Samples: %d\n  Bounces: %d\n", vars::config.sample_count, vars::config.bounce_limit);
            render_test_scene(out.data());
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
