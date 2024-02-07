
#ifndef TRIANGLE_CL_H
#define TRIANGLE_CL_H

#include "kernels/common.h"


typedef struct {
    float3 v0;
    float3 v1;
    float3 v2;
} rt_Triangle;


bool hitsTriangle(const rt_Triangle* triangle, const rt_Ray* ray, rt_HitRecord* record) {
    float3 v0v1 = triangle->v1 - triangle->v0;
    float3 v0v2 = triangle->v2 - triangle->v0;
    float3 pvec = cross(ray->direction, v0v2);
    float det = dot(v0v1, pvec);

    if (det < 0.001f && det > -0.001f) {
        return false;
    }

    float invDet = 1.0f / det;
    float3 tvec = ray->origin - triangle->v0;
    float u = dot(tvec, pvec) * invDet;
    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    float3 qvec = cross(tvec, v0v1);
    float v = dot(ray->direction, qvec) * invDet;
    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }

    float t = dot(v0v2, qvec) * invDet;
    if (t > 0.0f && t < record->hitDistance) {
        record->worldPosition = ray->origin + ray->direction * t;
        record->hitDistance = t;
        
        // change the normal's direction if its into the plane of triangle
        // record->worldNormal = normalize(cross(v0v1, v0v2));
        float3 normal = normalize(cross(v0v1, v0v2));
        record->worldNormal = dot(ray->direction, normal) > 0.0f ? -normal : normal;
        return true;
    }

    return false;
}


#endif
