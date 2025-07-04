#include "rng.glsl"

vec3 sample_uniform_hemisphere(inout uint state) {
    // Uniform random direction in a hemisphere
    float z = rng_next(state) * 2.0 - 1.0;
    float a = rng_next(state) * 6.2831853;
    float r = sqrt(1.0 - z * z);
    return vec3(r * cos(a), r * sin(a), z);
}