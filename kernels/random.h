
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


float randomNormalFloat(uint* seed) {
    float theta = 2.0f * M_PI_F * randomFloat(seed);
    float rho = sqrt(-2.0f * log(randomFloat(seed)));
    return rho * cos(theta);
}


float3 randomFloat3(uint* seed) {
    float3 p = {
        randomNormalFloat(seed),
        randomNormalFloat(seed),
        randomNormalFloat(seed)
    };
    return normalize(p);
}


#endif
