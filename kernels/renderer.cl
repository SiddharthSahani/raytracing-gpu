
#include "kernels/defines.h"
#include "kernels/common.h"
#include "kernels/random.h"
#include "kernels/objects.h"
#include "kernels/ray_gen.h"

#define MAX_OBJECTS 5
#define MAX_MATERIALS 3


typedef struct {
    rt_Object objects[MAX_OBJECTS];
    rt_Material materials[MAX_MATERIALS];
    float3 sky_color;
    uint object_count;
} rt_Scene;


float3 reflect(float3 I, float3 N) {
    return I - 2.0f * dot(N, I) * N;
}


rt_HitRecord traceRay(const rt_Ray* ray, local const rt_Scene* scene) {
    rt_HitRecord record;
    record.hitDistance = FLT_MAX;

    for (int i = 0; i < scene->object_count; i++) {
        const local rt_Object* object = &scene->objects[i];
        if (hitsObject(object, ray, &record)) {
            record.materialIndex = object->materialIndex;
        }
    }

    return record;
}


float3 perPixel(rt_Ray ray, local const rt_Scene* scene, uint* rngSeed) {
    float3 light = {0.0f, 0.0f, 0.0f};
    float3 contribution = {1.0f, 1.0f, 1.0f};

    for (int i = 0; i < CONFIG__BOUNCE_LIMIT; i++) {
        rngSeed += i * i * i;
        rt_HitRecord record = traceRay(&ray, scene);

        if (record.hitDistance == FLT_MAX) {
            light += scene->sky_color * contribution;
            break;
        }

        local const rt_Material* material = &scene->materials[record.materialIndex];

        contribution *= material->color;

        float3 diffuseDir = normalize(record.worldNormal + randomFloat3(rngSeed));
        float3 specularDir = reflect(ray.direction, record.worldNormal);
        ray.direction = normalize(mix(diffuseDir, specularDir, material->smoothness));
        ray.origin = record.worldPosition + record.worldNormal * 0.001f;
    }

    return light;
}


kernel void renderScene(const rt_Camera camera, const rt_Scene _scene, global PIXEL_BUFFER__TYPE* out, uint initialRngSeed) {
    local rt_Scene scene;

    if (get_local_id(0) == 0) {
        scene = _scene;
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    uint pixelIndex = get_global_id(0);

    uint rngSeed = (pixelIndex + 1) * initialRngSeed;

    float3 accumulatedFrameColor = {0.0f, 0.0f, 0.0f};

    rt_Ray ray = getRay(&camera, pixelIndex);

    for (int frameIndex = 0; frameIndex < CONFIG__SAMPLE_COUNT; frameIndex++) {
        rngSeed += frameIndex * 32421;
        accumulatedFrameColor += perPixel(ray, &scene, &rngSeed);
    }
    accumulatedFrameColor = accumulatedFrameColor / CONFIG__SAMPLE_COUNT;

#ifdef PIXEL_FORMAT__R32G32B32A32
    out[pixelIndex].xyz = accumulatedFrameColor;
    out[pixelIndex].w = 1.0f;
#endif
#ifdef PIXEL_FORMAT__R8G8B8A8
    accumulatedFrameColor = clamp(accumulatedFrameColor, 0.0f, 1.0f);
    out[pixelIndex].xyz = convert_uchar3(accumulatedFrameColor * 255.0f);
    out[pixelIndex].w = 255;
#endif

}
