
#ifndef COMMON_CL_H
#define COMMON_CL_H


typedef struct {
    float3 origin;
    float3 direction;
} rt_Ray;


typedef struct {
    float3 worldPosition;
    float3 worldNormal;
    float hitDistance;
    uint materialIndex;
} rt_HitRecord;


typedef struct {
    float3 color;
    float smoothness;
} rt_Material;


#endif
