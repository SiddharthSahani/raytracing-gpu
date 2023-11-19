
#include "kernels/common.h"
#include "kernels/random.h"
#include "kernels/objects.h"

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


void printSceneInfo(local const rt_Scene* scene) {
    bool using_materials[MAX_MATERIALS];
    for (int i = 0; i < MAX_MATERIALS; i++) {
        using_materials[i] = false;
    }

    printf("Number of objects: %d\n", scene->object_count);
    for (int i = 0; i < scene->object_count; i++) {
        local const rt_Object* object = &scene->objects[i];
        printf("Object %d:\n", i+1);
        printf("  internal: ");
        switch (object->type) {
            case OBJECT_TYPE_SPHERE:
                printSphereInfo(&object->sphere);
                break;
            case OBJECT_TYPE_TRIANGLE:
                printTriangleInfo(&object->triangle);
                break;
            default:
                printf("Invalid { type = %d }\n", object->type);
        }
        printf("  material-index: %d\n", object->material_idx);
        using_materials[object->material_idx] = true;
    }

    for (int i = 0; i < MAX_MATERIALS; i++) {
        if (using_materials[i]) {
            printf("Material Id: %d\n", i);
            printf(" color: {%f}\n", scene->materials[i].color);
        }
    }
}


rt_HitRecord traceRay(const rt_Ray* ray, local const rt_Scene* scene) {
    rt_HitRecord record;
    record.hit_distance = FLT_MAX;

    for (int i = 0; i < scene->object_count; i++) {
        if (hitsObject(&scene->objects[i], ray, &record)) {
            record.object_idx = i;
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

        local const rt_Object*object = &scene->objects[record.object_idx];
        local const rt_Material* material = &scene->materials[object->material_idx];

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

#   ifdef PRINT_SCENE_INFO
    if (pixel_idx == 0) {
        printSceneInfo(&scene);
    }
#   endif

    uint rng_seed = pixel_idx;

    float3 accumulated_color = {0.0f, 0.0f, 0.0f};
    
    rt_Ray ray;
    ray.origin = camera_position;
    ray.direction = ray_directions[pixel_idx];

    const int frames = config.sample_count;
    for (int frame_idx = 0; frame_idx < frames; frame_idx++) {
        rng_seed += frame_idx * 32421;
        accumulated_color += clamp(perPixel(ray, &scene, &rng_seed, &config), 0.0f, 1.0f);
    }

    out[pixel_idx].xyz = accumulated_color / frames;
    out[pixel_idx].w = 1.0f;
}
