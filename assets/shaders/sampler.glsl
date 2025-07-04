#ifndef TINT_SAMPLER
#define TINT_SAMPLER

#include "rng.glsl"

// Samples a random unit vector from the unit hemisphere oriented z-up
vec3 sample_uniform_hemisphere(inout uint state) {
    float z = rng_next(state) * 2.0 - 1.0;
    float a = rng_next(state) * 6.2831853;
    float r = sqrt(1.0 - z * z);
    return vec3(r * cos(a), r * sin(a), z);
}

#endif