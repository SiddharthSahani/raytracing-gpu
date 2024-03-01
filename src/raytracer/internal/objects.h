
#pragma once

#include <CL/cl.hpp>


namespace rt::internal {

struct Sphere {
    cl_float3 position;
    cl_float radius;
};


struct Triangle {
    cl_float3 v0;
    cl_float3 v1;
    cl_float3 v2;
};


struct Object {
    union {
        Sphere sphere;
        Triangle triangle;
    };
    cl_uint type;
    cl_uint materialIndex;
};

}
