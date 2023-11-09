
typedef struct {
    float3 origin;
    float3 direction;
} rt_Ray;


typedef struct {
    float3 world_position;
    float3 world_normal;
    float hit_distance;
} rt_HitRecord;


void hit_sphere(rt_Ray ray, rt_HitRecord* record) {
    float3 sphere_position = {0.0f, 0.0f, 0.0f};
    float sphere_radius = 1.0f;

    float3 oc = ray.origin - sphere_position;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0f * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere_radius * sphere_radius;
    float d = b*b - 4*a*c;

    if (d < 0.0f) {
        return;
    }

    float t = (-b - sqrt(d)) / (2.0f * a);

    if (t < record->hit_distance) {
        record->world_position = oc + ray.direction * t;
        record->world_normal = normalize(record->world_position);
        record->hit_distance = t;
        record->world_position += sphere_position;
    }
}


kernel void renderScene(global const rt_Ray* rays, global float4* out) {
    size_t i = get_global_id(0);

    const float4 background_color = {0, 1, 1, 1};

    rt_HitRecord record;
    record.hit_distance = FLT_MAX;
    hit_sphere(rays[i], &record);

    if (record.hit_distance == FLT_MAX) {
        out[i] = background_color;
    } else {
        out[i].xyz = record.world_normal;
        out[i].w = 1.0f;
    }
}
