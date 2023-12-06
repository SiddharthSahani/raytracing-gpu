
#ifndef SPHERE_CL_H
#define SPHERE_CL_H

#include "kernels/common.h"


typedef struct {
    float3 position;
    float radius;
} rt_Sphere;


bool hitsSphere(local const rt_Sphere* sphere, const rt_Ray* ray, rt_HitRecord* record) {
    float3 oc = ray->origin - sphere->position;
    float a = dot(ray->direction, ray->direction);
    float b = 2.0f * dot(oc, ray->direction);
    float c = dot(oc, oc) - sphere->radius * sphere->radius;
    float d = b*b - 4*a*c;

    if (d < 0.0f) {
        return false;
    }

    float t = (-b - sqrt(d)) / (2.0f * a);

    if (t > 0.0f && t < record->hitDistance) {
        record->worldPosition = oc + ray->direction * t;
        record->worldNormal = normalize(record->worldPosition);
        record->hitDistance = t;
        record->worldPosition += sphere->position;
        return true;
    }

    return false;
}


#endif
