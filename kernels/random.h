
#ifndef RANDOM_CL_H
#define RANDOM_CL_H


uint pcg_hash(uint input) {
    uint state = input * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state);
    return (word >> 22u) ^ word;
}


float random_float(uint* seed) {
    *seed = pcg_hash(*seed);
    return (float) *seed / UINT_MAX;
}


float3 random_float3(uint* seed) {
    float3 x = {
        random_float(seed),
        random_float(seed),
        random_float(seed)
    };
    return normalize(x * 2.0f - 1.0f);
}


#endif
