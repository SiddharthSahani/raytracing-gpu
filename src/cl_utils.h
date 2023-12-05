
#pragma once

#include <CL/opencl.hpp>
#include <vector>
#include <fstream>
#include <sstream>


bool chooseClDevice(cl::Device& selectedDevice) {
    std::vector<cl::Platform> allPlatforms;
    cl::Platform::get(&allPlatforms);

    std::vector<cl::Device> allDevices;
    for (const auto& plat : allPlatforms) {
        std::vector<cl::Device> temp;
        plat.getDevices(CL_DEVICE_TYPE_ALL, &temp);
        allDevices.insert(allDevices.end(), temp.begin(), temp.end());
    }

    if (allDevices.size() == 0) {
        printf("No OpenCl compactible device found\n");
        return false;
    }
    if (allDevices.size() == 1) {
        selectedDevice = allDevices[0];
        return true;
    }

    printf("Multiple devices found, choose one:\n");
    for (int i = 0; i < allDevices.size(); i++) {
        const auto& device = allDevices[i];
        printf("%d) %s [%d CU]\n", i+1, device.getInfo<CL_DEVICE_NAME>().c_str(), device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>());
    }

    int selectedDeviceIdx;
    printf("Enter selection: ");
    scanf("%d", &selectedDeviceIdx);

    selectedDevice = allDevices[selectedDeviceIdx-1];
    return true;
}


void printDeviceInfo(cl::Device& device) {
    auto cuCount = device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
    auto clockFreq = device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>();
    auto deviceName = device.getInfo<CL_DEVICE_NAME>();
    auto version = device.getInfo<CL_DEVICE_VERSION>();
    auto clVersion = device.getInfo<CL_DEVICE_OPENCL_C_VERSION>();

    printf("----- Selected Device Info -----\n");
    printf("  Name: %s\n", deviceName.c_str());
    printf("  Version: %s\n", version.c_str());
    printf("  OpenCl C Version: %s\n", clVersion.c_str());
    printf("  Compute Units: %d\n", cuCount);
    printf("  Clock Frequency: %d\n", clockFreq);
}


std::string readFile(const char* filepath) {
    std::ifstream file(filepath, std::ios::in | std::ios::ate);

    int fileSize = file.tellg();
    std::string fileContents(fileSize, '\0');

    file.seekg(0);
    file.read(&fileContents[0], fileSize);
    return fileContents;
}
