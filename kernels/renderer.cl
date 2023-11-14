
#include "kernels/common.h"
#include "kernels/sphere.h"
#include "kernels/random.h"

#define MAX_SPHERES 5
#define MAX_MATERIALS 5


typedef struct {
    uint num_spheres;
    float3 sky_color;
    rt_Sphere spheres[MAX_SPHERES];
    rt_Material materials[MAX_MATERIALS];
} rt_Scene;


typedef struct {
    uint samples;
    uint bounces;
} rt_Config;


rt_HitRecord traceRay(const rt_Ray* ray, local const rt_Scene* scene) {
    rt_HitRecord record;
    record.hit_distance = FLT_MAX;

    for (int i = 0; i < scene->num_spheres; i++) {
        if (hitsSphere(&scene->spheres[i], ray, &record)) {
            record.sphere_idx = i;
        }
    }

    return record;
}


float3 perPixel(rt_Ray ray, local const rt_Scene* scene, uint* rng_seed, local const rt_Config* config) {
    float3 light = {0.0f, 0.0f, 0.0f};
    float3 contribution = {1.0f, 1.0f, 1.0f};

    for (int i = 0; i < config->bounces; i++) {
        rng_seed += i * i * i;
        rt_HitRecord record = traceRay(&ray, scene);

        if (record.hit_distance == FLT_MAX) {
            light += scene->sky_color * contribution;
            break;
        }

        local const rt_Sphere* sphere = &scene->spheres[record.sphere_idx];
        local const rt_Material* material = &scene->materials[sphere->material_idx];

        contribution *= material->color;

        ray.origin = record.world_position + record.world_normal * 0.001f;
        ray.direction = normalize(record.world_normal + randomFloat3(rng_seed));
    }

    return light;
}


kernel void renderScene(
    const float3 camera_position, global const float3* ray_directions,
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

    size_t pixel_idx = get_global_id(0);

    uint rng_seed = pixel_idx;

    float3 accumulated_color = {0.0f, 0.0f, 0.0f};
    
    rt_Ray ray;
    ray.origin = camera_position;
    ray.direction = ray_directions[pixel_idx];

    const int frames = config.samples;
    for (int frame_idx = 0; frame_idx < frames; frame_idx++) {
        rng_seed += frame_idx * 32421;
        accumulated_color += clamp(perPixel(ray, &scene, &rng_seed, &config), 0.0f, 1.0f);
    }

    out[pixel_idx].xyz = accumulated_color / frames;
    out[pixel_idx].w = 1.0f;
}
