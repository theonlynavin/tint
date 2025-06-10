#ifndef TINT_RAY
#define TINT_RAY

typedef struct {
    float3 origin;
    float3 direction;
    float t_min;
    float t_max;
} Ray;

Ray create_ray(float3 origin, float3 direction) {
    Ray r;
    r.origin = origin;
    r.direction = direction;
    r.t_max = 1e8f;
    r.t_min = 0;
    return r;
}

float3 point_at(Ray r, float t) {
    return r.origin + t * r.direction;
}

#endif