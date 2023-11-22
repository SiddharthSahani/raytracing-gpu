
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


typedef struct {
    uint sample_count;
    uint bounce_limit;
} rt_Config;


float3 reflect(float3 I, float3 N) {
    return I - 2.0f * dot(N, I) * N;
}


rt_HitRecord traceRay(const rt_Ray* ray, local const rt_Scene* scene) {
    rt_HitRecord record;
    record.hit_distance = FLT_MAX;

    for (int i = 0; i < scene->object_count; i++) {
        const local rt_Object* object = &scene->objects[i];
        if (hitsObject(object, ray, &record)) {
            record.material_idx = object->material_idx;
        }
    }

    return record;
}


float3 perPixel(rt_Ray ray, local const rt_Scene* scene, uint* rng_seed, local const rt_Config* config) {
    float3 light = {0.0f, 0.0f, 0.0f};
    float3 contribution = {1.0f, 1.0f, 1.0f};

    for (int i = 0; i < config->bounce_limit; i++) {
        rng_seed += i * i * i;
        rt_HitRecord record = traceRay(&ray, scene);

        if (record.hit_distance == FLT_MAX) {
            light += scene->sky_color * contribution;
            break;
        }

        local const rt_Material* material = &scene->materials[record.material_idx];

        contribution *= material->color;

        float3 diffuse_dir = record.world_normal + randomFloat3(rng_seed);
        float3 specular_dir = reflect(ray.direction, record.world_normal);
        ray.direction = normalize(mix(diffuse_dir, specular_dir, material->smoothness));
        ray.origin = record.world_position + record.world_normal * 0.001f;
    }

    return light;
}


kernel void renderScene(
    const rt_Camera camera,
    const rt_Scene _scene, const rt_Config _config,
    global float4* out
) {
    local rt_Scene scene;
    local rt_Config config;

    if (get_local_id(0) == 0) {
        scene = _scene;
        config = _config;
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    uint pixel_idx = get_global_id(0);

    uint rng_seed = pixel_idx;

    float3 accumulated_color = {0.0f, 0.0f, 0.0f};

    rt_Ray ray = getRay(&camera, pixel_idx);

    const int frames = config.sample_count;
    for (int frame_idx = 0; frame_idx < frames; frame_idx++) {
        rng_seed += frame_idx * 32421;
        accumulated_color += clamp(perPixel(ray, &scene, &rng_seed, &config), 0.0f, 1.0f);
    }

    out[pixel_idx].xyz = accumulated_color / frames;
    out[pixel_idx].w = 1.0f;
}
