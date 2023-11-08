
typedef struct {
    float3 origin;
    float3 direction;
} rt_Ray;


bool hit_sphere(rt_Ray ray) {
    float3 sphere_position = {0.0f, 0.0f, 0.0f};
    float sphere_radius = 1.0f;

    float3 oc = ray.origin - sphere_position;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0f * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere_radius * sphere_radius;
    float d = b*b - 4*a*c;

    return (d > 0.0f);
}


kernel void renderScene(global const rt_Ray* rays, global float4* out) {
    size_t i = get_global_id(0);

    float4 intersection_color = {1, 0, 0, 1};
    float4 background_color = {0, 0, 1, 1};

    if (hit_sphere(rays[i])) {
        out[i] = intersection_color;
    } else {
        out[i] = background_color;
    }
}
