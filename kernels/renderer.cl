
typedef struct {
    float3 position;
    float radius;
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


constant rt_Sphere spheres[] = {
    {{0, 0, 0}, 1},
    {{0, -6, 0}, 5}
};


void hit_sphere(constant rt_Sphere* sphere, rt_Ray ray, rt_HitRecord* record) {
    float3 oc = ray.origin - sphere->position;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0f * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere->radius * sphere->radius;
    float d = b*b - 4*a*c;

    if (d < 0.0f) {
        return;
    }

    float t = (-b - sqrt(d)) / (2.0f * a);

    if (t < record->hit_distance) {
        record->world_position = oc + ray.direction * t;
        record->world_normal = normalize(record->world_position);
        record->hit_distance = t;
        record->world_position += sphere->position;
    }
}


kernel void renderScene(global const rt_Ray* rays, global float4* out) {
    size_t i = get_global_id(0);
    if (i == 0) {
        printf("Sphere[0] = {%f, %f}\n", spheres[0].position, spheres[0].radius);
        printf("Sphere[1] = {%f, %f}\n", spheres[1].position, spheres[1].radius);
    }

    const float4 background_color = {0, 1, 1, 1};

    rt_HitRecord record;
    record.hit_distance = FLT_MAX;

    for (int obj_idx = 0; obj_idx < 2; obj_idx++) {
        hit_sphere(&spheres[obj_idx], rays[i], &record);
    }

    if (record.hit_distance == FLT_MAX) {
        out[i] = background_color;
    } else {
        out[i].xyz = record.world_normal;
        out[i].w = 1.0f;
    }
}
