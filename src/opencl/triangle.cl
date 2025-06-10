#ifndef TINT_TRIANGLE
#define TINT_TRIANGLE

#include "ray.cl"

float3 intersect_triangle(float3 v0, float3 v1, float3 v2, Ray ray) {
    
    float3 e10 = v1 - v0;
    float3 e20 = v2 - v0;
    float3 pvec = cross(ray.direction, e20);
    float det = dot(e10, pvec);

    if (det < FLT_EPSILON)
        return (float3)(-1, 0, 0);

    float inv_det = 1.0 / det;
    float3 tvec = ray.origin - v0;
    float u = inv_det * dot(tvec, pvec);

    if (u < 0 || u > 1)
        return (float3)(-1, 0, 0);

    float3 qvec = cross(tvec, e10);
    float v = inv_det * dot(ray.direction, qvec);

    if (v < 0 || u + v > 1)
        return (float3)(-1, 0, 0);

    float t = inv_det * dot(e20, qvec);

    if (t > ray.t_min && t < ray.t_max)
    {
        float3 tuv;
        tuv.x = t;
        tuv.y = u;
        tuv.z = v;
        return tuv;
    }

    return (float3)(-1, 0, 0);
}

float3 get_triangle_normal(float3 n0, float3 n1, float3 n2, float2 uv) {
    return normalize(uv.x * n1 + uv.y * n2 + (1 - uv.x - uv.y) * n0);
}

float3 get_triangle_point(float3 v0, float3 v1, float3 v2, float2 uv) {
    return normalize(uv.x * v1 + uv.y * v2 + (1 - uv.x - uv.y) * v0);
}

#endif