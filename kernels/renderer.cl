
#include "kernels/defines.h"
#include "kernels/common.h"
#include "kernels/random.h"
#include "kernels/objects.h"
#include "kernels/ray_gen.h"


typedef struct {
    float3 backgroundColor;
    uint objectCount;
} rt_SceneParams;


float3 reflect(float3 I, float3 N) {
    return I - 2.0f * dot(N, I) * N;
}


rt_HitRecord traceRay(const rt_Ray* ray, const rt_SceneParams* scene, global const rt_Object* objects) {
    rt_HitRecord record;
    record.hitDistance = FLT_MAX;

    for (int i = 0; i < scene->objectCount; i++) {
        global const rt_Object* object = &objects[i];
        if (hitsObject(object, ray, &record)) {
            record.materialIndex = object->materialIndex;
        }
    }

    return record;
}


float3 perPixel(rt_Ray ray, const rt_SceneParams* scene, global const rt_Object* objects, global const rt_Material* materials, uint* rngSeed) {
    float3 light = {0.0f, 0.0f, 0.0f};
    float3 contribution = {1.0f, 1.0f, 1.0f};

    for (int i = 0; i < CONFIG__BOUNCE_LIMIT; i++) {
        rngSeed += i * i * i;
        rt_HitRecord record = traceRay(&ray, scene, objects);

        if (record.hitDistance == FLT_MAX) {
            light += scene->backgroundColor * contribution;
            break;
        }

        global const rt_Material* material = &materials[record.materialIndex];

        light += material->emissionColor * contribution;
        contribution *= material->color;

        float3 diffuseDir = normalize(record.worldNormal + randomFloat3(rngSeed));
        float3 specularDir = reflect(ray.direction, record.worldNormal);
        ray.origin = record.worldPosition + record.worldNormal * 0.001f;
        ray.direction = normalize(mix(diffuseDir, specularDir, material->smoothness));
    }

    return light;
}


kernel void renderScene(
    const rt_Camera camera,
    const rt_SceneParams scene,
    global const rt_Object* objects,
    global const rt_Material* materials,
    uint initialRngSeed,
    global PIXEL_BUFFER__TYPE* out
) {
    uint pixelIndex = get_global_id(0);

    uint rngSeed = (pixelIndex + 1) * initialRngSeed;

    float3 accumulatedFrameColor = {0.0f, 0.0f, 0.0f};

    rt_Ray ray = getRay(&camera, pixelIndex);

    for (int frameIndex = 0; frameIndex < CONFIG__SAMPLE_COUNT; frameIndex++) {
        rngSeed += frameIndex * 32421;
        accumulatedFrameColor += perPixel(ray, &scene, objects, materials, &rngSeed);
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
