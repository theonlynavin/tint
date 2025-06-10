#ifndef TINT_CAMERA
#define TINT_CAMERA

#include "ray.cl"
#include "random.cl"

typedef struct {
    float3 fwd;
    float3 up;
    float3 right;
    float3 origin;
    float focal_length;
    float aperture;
    float aspect;
    float scale;
} Camera;

Ray create_camera_ray(Camera cam, float u, float v, PCG32State* rng) {

    float x = (2 * u - 1) * cam.scale;
    float y = (2 * v - 1) * cam.scale * cam.aspect;

    float3 direction = normalize((cam.right * x) + (cam.up * y) + (cam.fwd * -1));

    // simple lens defocus
    float2 disc = random_uniform_disc(rng) * cam.aperture;
    float3 at = cam.origin + cam.focal_length * direction;
    float3 from = cam.origin + (float3)(disc.x, disc.y, 0);

    return create_ray(from, normalize(at - from));
}

#endif