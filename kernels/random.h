
#ifndef RANDOM_CL_H
#define RANDOM_CL_H


uint pcgHash(uint input) {
    uint state = input * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state);
    return (word >> 22u) ^ word;
}


float randomFloat(uint* seed) {
    *seed = pcgHash(*seed);
    return (float) *seed / UINT_MAX;
}


float3 randomFloat3(uint* seed) {
    float3 x = {
        randomFloat(seed),
        randomFloat(seed),
        randomFloat(seed)
    };
    return normalize(x * 2.0f - 1.0f);
}


#endif
