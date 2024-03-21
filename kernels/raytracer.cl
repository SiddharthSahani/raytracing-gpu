
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
        const rt_Object object = objects[i];
        if (hitsObject(object, ray, &record)) {
            record.materialIndex = object.materialIndex;
        }
    }

    return record;
}


float3 perPixel(rt_Ray ray, const rt_SceneParams* scene, global const rt_Object* objects, global const rt_Material* materials, uint* rngSeed, uint maxBounces, uint* bounces) {
    float3 light = {0.0f, 0.0f, 0.0f};
    float3 contribution = {1.0f, 1.0f, 1.0f};

    uint i;
    for (i = 0; i < maxBounces; i++) {
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

    *bounces += i;
    return light;
}


kernel void raytraceScene(
    // Camera
    const rt_Camera camera,
    // Scene Info
    const rt_SceneParams scene,
    global const rt_Object* objects,
    global const rt_Material* materials,
    // Random number generation
    uint initialRngSeed,
    // Config
    uint sampleCount,
    uint maxBounces,
    // Out texture
    write_only image2d_t out,
    // Bounce Height Map
    write_only image2d_t bounceImage
) {
    uint pixelIndex = get_global_id(0);

    uint rngSeed = (pixelIndex + 1) * initialRngSeed;
    uint bounces = 0;

    rt_Ray ray = getRay(&camera, pixelIndex);

    float3 accumulatedFrameColor = {0.0f, 0.0f, 0.0f};

    for (int frameIndex = 0; frameIndex < sampleCount; frameIndex++) {
        rngSeed += frameIndex * 32421;
        accumulatedFrameColor += perPixel(ray, &scene, objects, materials, &rngSeed, maxBounces, &bounces);
    }
    accumulatedFrameColor = accumulatedFrameColor / sampleCount;

    int2 imgCoords = {pixelIndex % camera.imageSize.x, pixelIndex / camera.imageSize.x};
    float4 imgColor = {accumulatedFrameColor.xyz, 1.0f};
    write_imagef(out, imgCoords, imgColor);

    write_imagef(bounceImage, imgCoords, (float4) bounces / (sampleCount * maxBounces));
}
