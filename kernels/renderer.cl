
#include "kernels/random.h"


typedef struct {
    float3 position;
    float radius;
    float3 color;
} rt_Sphere;


typedef struct {
    float3 origin;
    float3 direction;
} rt_Ray;


typedef struct {
    float3 world_position;
    float3 world_normal;
    float hit_distance;
    uint sphere_idx;
} rt_HitRecord;


#define MAX_SPHERES 5

typedef struct {
    uint num_spheres;
    rt_Sphere spheres[MAX_SPHERES];
    float3 sky_color;
} rt_Scene;


bool hit_sphere(const rt_Sphere* sphere, const rt_Ray* ray, rt_HitRecord* record) {
    float3 oc = ray->origin - sphere->position;
    float a = dot(ray->direction, ray->direction);
    float b = 2.0f * dot(oc, ray->direction);
    float c = dot(oc, oc) - sphere->radius * sphere->radius;
    float d = b*b - 4*a*c;

    if (d < 0.0f) {
        return false;
    }

    float t = (-b - sqrt(d)) / (2.0f * a);

    if (t > 0.0f && t < record->hit_distance) {
        record->world_position = oc + ray->direction * t;
        record->world_normal = normalize(record->world_position);
        record->hit_distance = t;
        record->world_position += sphere->position;
        return true;
    }

    return false;
}


rt_HitRecord trace_ray(const rt_Ray* ray, const rt_Scene* scene) {
    rt_HitRecord record;
    record.hit_distance = FLT_MAX;

    for (int i = 0; i < scene->num_spheres; i++) {
        if (hit_sphere(&scene->spheres[i], ray, &record)) {
            record.sphere_idx = i;
        }
    }

    return record;
}


float3 per_pixel(rt_Ray ray, const rt_Scene scene, uint* rng_seed) {
    float3 light = {0.0f, 0.0f, 0.0f};
    float3 contribution = {1.0f, 1.0f, 1.0f};

    const int bounces = 10;
    for (int i = 0; i < bounces; i++) {
        rng_seed += i * i * i;
        rt_HitRecord record = trace_ray(&ray, &scene);

        if (record.hit_distance == FLT_MAX) {
            light += scene.sky_color * contribution;
            break;
        }

        const rt_Sphere* sphere = &scene.spheres[record.sphere_idx];

        contribution *= sphere->color;
        
        ray.origin = record.world_position + record.world_normal * 0.001f;
        ray.direction = normalize(record.world_normal + random_float3(rng_seed));
    }

    return light;
}


kernel void renderScene(global const rt_Ray* rays, const rt_Scene scene, global float4* out) {
    size_t pixel_idx = get_global_id(0);

    uint rng_seed = pixel_idx;

    float3 accumulated_color = {0.0f, 0.0f, 0.0f};
    
    const int frames = 100;
    for (int frame_idx = 0; frame_idx < frames; frame_idx++) {
        rng_seed += frame_idx * 32421;
        accumulated_color += clamp(per_pixel(rays[pixel_idx], scene, &rng_seed), 0.0f, 1.0f);
    }

    out[pixel_idx].xyz = accumulated_color / frames;
    out[pixel_idx].w = 1.0f;
}
