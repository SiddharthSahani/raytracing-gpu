
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
    uint object_idx;
} rt_HitRecord;


typedef struct {
    float3 color;
    float smoothness;
} rt_Material;


#endif
