
#pragma once

#include <CL/opencl.hpp>
#include <vector>
#include <fstream>


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


void create_opencl_objects(const cl::Device& device, cl::Context& context, cl::CommandQueue& queue, cl::Program& program) {
    context = cl::Context(device);
    queue = cl::CommandQueue(context, device);

    std::string program_string = read_file("kernels/renderer.cl");
    program = cl::Program(program_string);
}


bool build_program(const cl::Program& program, const cl::Device& device, bool print_scene_info = false) {
    std::string program_build_options = "";
    if (print_scene_info) {
        program_build_options += "-DPRINT_SCENE_INFO";
    }

    if (program.build(program_build_options.c_str())) {
        printf("Unable to build OpenCl program\n");
        std::string build_log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
        printf("Build Log:\n%s\n", build_log.c_str());
        return false;
    }

    return true;
}
