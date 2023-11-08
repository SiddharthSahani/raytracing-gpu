
#include <CL/opencl.hpp>
#include <raylib/raylib.h>
#include <vector>


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


int main() {
    cl::Device device;
    printf("OpenCl device selected completed: %d\n", get_device(device));
    print_device_info(device);

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(1280, 720, "OpenCl with raylib");
    SetTargetFPS(30);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
}
