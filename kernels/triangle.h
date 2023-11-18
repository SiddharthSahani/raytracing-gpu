
#ifndef TRIANGLE_CL_H
#define TRIANGLE_CL_H

#include "kernels/common.h"


typedef struct {
    float3 v0;
    float3 v1;
    float3 v2;
} rt_Triangle;


bool hitsTriangle(local const rt_Triangle* triangle, const rt_Ray* ray, rt_HitRecord* record) {
    float3 v0v1 = triangle->v1 - triangle->v0;
    float3 v0v2 = triangle->v2 - triangle->v0;
    float3 pvec = cross(ray->direction, v0v2);
    float det = dot(v0v1, pvec);

    if (det < 0.001f && det > -0.001f) {
        return false;
    }

    float inv_det = 1.0f / det;
    float3 tvec = ray->origin - triangle->v0;
    float u = dot(tvec, pvec) * inv_det;
    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    float3 qvec = cross(tvec, v0v1);
    float v = dot(ray->direction, qvec) * inv_det;
    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }

    float t = dot(v0v2, qvec) * inv_det;
    if (t > 0.0f && t < record->hit_distance) {
        record->world_position = ray->origin + ray->direction * t;
        record->hit_distance = t;
        
        // change the normal's direction if its into the plane of triangle
        // record->world_normal = normalize(cross(v0v1, v0v2));
        float3 normal = normalize(cross(v0v1, v0v2));
        record->world_normal = dot(ray->direction, normal) > 0.0f ? -normal : normal;
        return true;
    }

    return false;
}


void printTriangleInfo(local const rt_Triangle* triangle) {
    printf("Triangle {\n");
    printf("    v0: {%f}\n", triangle->v0);
    printf("    v1: {%f}\n", triangle->v1);
    printf("    v2: {%f}\n", triangle->v2);
    printf("  }\n");
}


#endif
