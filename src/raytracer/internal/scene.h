
#pragma once

#include <CL/opencl.hpp>


namespace rt::internal {

// temp thing
struct SceneExtra {
    cl_float3 backgroundColor;
    cl_uint numObjects;
};


struct Scene {
    cl::Buffer objectsBuffer;
    cl::Buffer materialsBuffer;
    SceneExtra extra;
};

}
