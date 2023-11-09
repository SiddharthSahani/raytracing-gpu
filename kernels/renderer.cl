
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

    if (t < record->hit_distance) {
        record->world_position = oc + ray->direction * t;
        record->world_normal = normalize(record->world_position);
        record->hit_distance = t;
        record->world_position += sphere->position;
        return true;
    }

    return false;
}


float3 per_pixel(rt_Ray ray, const rt_Scene scene) {
    rt_HitRecord record;
    record.hit_distance = FLT_MAX;

    float3 sphere_color;

    for (int i = 0; i < scene.num_spheres; i++) {
        if (hit_sphere(&scene.spheres[i], &ray, &record)) {
            sphere_color = scene.spheres[i].color;
        }
    }

    if (record.hit_distance == FLT_MAX) {
        return scene.sky_color;
    } else {
        float3 light_direction = {-1.0f, -1.0f, -1.0f};
        light_direction = normalize(light_direction);
        float light_intensity = max(0.0f, dot(record.world_normal, -light_direction));

        sphere_color *= light_intensity;
        return sphere_color;
    }
}


kernel void renderScene(global const rt_Ray* rays, const rt_Scene scene, global float4* out) {
    size_t i = get_global_id(0);

    out[i].xyz = per_pixel(rays[i], scene);
    out[i].w = 1.0f;
}
