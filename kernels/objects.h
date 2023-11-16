
#ifndef OBJECTS_CL_H
#define OBJECTS_CL_H

#include "kernels/sphere.h"

#define OBJECT_TYPE_SPHERE 0


typedef struct {
    union {
        rt_Sphere sphere;
    };
    uint type;
    uint material_idx;
} rt_Object;


bool hitsObject(local const rt_Object* object, const rt_Ray* ray, rt_HitRecord* record) {
    switch (object->type) {
        case OBJECT_TYPE_SPHERE:
            return hitsSphere(&object->sphere, ray, record);
        default:
            return false;
    }
}


#endif
