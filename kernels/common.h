
#ifndef COMMON_CL_H
#define COMMON_CL_H


typedef struct {
    float3 origin;
    float3 direction;
} rt_Ray;


typedef struct {
    float3 world_position;
    float3 world_normal;
    float hit_distance;
    uint sphere_idx;
} rt_HitRecord;


typedef struct {
    float3 color;
} rt_Material;


#endif
