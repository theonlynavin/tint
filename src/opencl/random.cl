#ifndef TINT_RANDOM
#define TINT_RANDOM

typedef struct {
    uint state;
    uint inc;
} PCG32State;

inline uint pcg32_next(PCG32State* rng) {
    uint oldstate = rng->state;
    rng->state = oldstate * 747796405u + rng->inc;
    uint xorshifted = ((oldstate >> ((oldstate >> 28u) + 4u)) ^ oldstate) >> 1;
    uint rot = oldstate >> 27u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

inline float random_float(PCG32State* rng) {
    return (float)(pcg32_next(rng) & 0x00FFFFFF) / 16777216.0f;
}

inline float random_uniform_1d(PCG32State* rng) {
    return random_float(rng);
}

inline float2 random_uniform_2d(PCG32State* rng) {
    return (float2)(random_float(rng), random_float(rng));
}

inline float3 random_uniform_3d(PCG32State* rng) {
    return (float3)(random_float(rng), random_float(rng), random_float(rng));
}

inline float2 random_uniform_disc(PCG32State* rng) {
    
    float u1 = random_float(rng);
    float u2 = random_float(rng);
    
    float2 p = (float2)(2.0f * u1 - 1.0f, 2.0f * u2 - 1.0f);

    if (p.x == 0 && p.y == 0)
        return (float2)(0.0f, 0.0f);

    float r, theta;
    if (fabs(p.x) > fabs(p.y)) {
        r = p.x;
        theta = (M_PI_F / 4.0f) * (p.y / p.x);
    } else {
        r = p.y;
        theta = (M_PI_F / 2.0f) - (M_PI_F / 4.0f) * (p.x / p.y);
    }

    return r * (float2)(cos(theta), sin(theta));
}

#endif