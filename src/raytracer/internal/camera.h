
#pragma once

#include <CL/cl.hpp>


namespace rt::internal {

struct Camera {
    cl_float16 invViewMat;
    cl_float16 invProjMat;
    cl_float3 position;
    cl_uint2 imageSize;
};

}
