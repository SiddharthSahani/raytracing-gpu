
#pragma once

#include <CL/opencl.hpp>


namespace rt::internal {

struct Material {
    cl_float3 color;
    cl_float3 emissionColor;
    cl_float smoothness;
};

}
