
#ifndef OBJECTS_CL_H
#define OBJECTS_CL_H

#include "kernels/sphere.h"
#include "kernels/triangle.h"

#define OBJECT_TYPE_SPHERE 0
#define OBJECT_TYPE_TRIANGLE 1


typedef struct {
    union {
        rt_Sphere sphere;
        rt_Triangle triangle;
    };
    uint type;
    uint materialIndex;
} rt_Object;


bool hitsObject(local const rt_Object* object, const rt_Ray* ray, rt_HitRecord* record) {
    switch (object->type) {
        case OBJECT_TYPE_SPHERE:
            return hitsSphere(&object->sphere, ray, record);
        case OBJECT_TYPE_TRIANGLE:
            return hitsTriangle(&object->triangle, ray, record);
        default:
            return false;
    }
}


#endif
