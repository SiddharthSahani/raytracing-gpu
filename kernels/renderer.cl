
typedef struct {
    float3 origin;
    float3 direction;
} rt_Ray;


float hit_sphere(rt_Ray ray) {
    float3 sphere_position = {0.0f, 0.0f, 0.0f};
    float sphere_radius = 1.0f;

    float3 oc = ray.origin - sphere_position;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0f * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere_radius * sphere_radius;
    float d = b*b - 4*a*c;

    if (d > 0.0f) {
        return (-b - sqrt(d)) / (2.0f * a);
    } else {
        return -1.0f;
    }
}


kernel void renderScene(global const rt_Ray* rays, global float4* out) {
    size_t i = get_global_id(0);

    float4 intersection_color = {1, 0, 0, 1};
    float4 background_color = {0, 1, 1, 1};

    float t = hit_sphere(rays[i]);
    if (t != -1.0f) {
        float3 n = normalize(rays[i].origin + rays[i].direction * t);
        out[i].xyz = n.xyz;
        out[i].w = 1.0f;
    } else {
        out[i] = background_color;
    }
}
