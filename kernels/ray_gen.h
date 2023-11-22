
#ifndef RAYGEN_CL_H
#define RAYGEN_CL_H

#include "kernels/common.h"


typedef struct {
    float16 inv_view;
    float16 inv_projection;
    float3 position;
    uint2 image_size;
} rt_Camera;


float4 mul_f16_f4(float16 matrix, float4 vector) {
    float4 result;
    result[0] = matrix[0*4+0] * vector[0] + matrix[1*4+0] * vector[1] + matrix[2*4+0] * vector[2] + matrix[3*4+0] * vector[3];
    result[1] = matrix[0*4+1] * vector[0] + matrix[1*4+1] * vector[1] + matrix[2*4+1] * vector[2] + matrix[3*4+1] * vector[3];
    result[2] = matrix[0*4+2] * vector[0] + matrix[1*4+2] * vector[1] + matrix[2*4+2] * vector[2] + matrix[3*4+2] * vector[3];
    result[3] = matrix[0*4+3] * vector[0] + matrix[1*4+3] * vector[1] + matrix[2*4+3] * vector[2] + matrix[3*4+3] * vector[3];
    return result;
}


rt_Ray getRay(const rt_Camera* camera, uint pixel_index) {
    float2 pixel_coord = {pixel_index % camera->image_size.x, pixel_index / camera->image_size.x};
    pixel_coord.y = camera->image_size.y - pixel_coord.y;

    float2 coord = pixel_coord / convert_float2(camera->image_size) * 2.0f - 1.0f; 
    float4 target = mul_f16_f4(camera->inv_projection, (float4)(coord.xy, 1.0f, 1.0f));

    rt_Ray ray;
    ray.origin = camera->position;
    ray.direction = mul_f16_f4(camera->inv_view, (float4)(normalize(target.xyz / target.w), 0.0f)).xyz;
    return ray;
}


#endif
