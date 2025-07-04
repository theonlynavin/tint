#ifndef TINT_CAMERA
#define TINT_CAMERA

struct Camera {
    vec3 position;
    vec3 fwd;
    vec3 up;
    vec3 right;
    float scale; // tan(half_fov)
    float aspect_ratio;
    float aperture;
    float focal_length;
};

struct Ray {
    vec3 origin;
    vec3 direction;
};

// Generates a ray given NDC coordinates
Ray generate_ray(Camera cam, vec2 uv, inout uint rng_state) {

    float x = (2 * uv.x - 1) * cam.scale;
    float y = (2 * uv.y - 1) * cam.scale * cam.aspect_ratio;

    vec3 direction = normalize((cam.right * x) + (cam.up * y) + (cam.fwd * -1));

    // Simple lens defocus
    vec3 disc = vec3(rng_next(rng_state) - 0.5, rng_next(rng_state) - 0.5, 0) * cam.aperture;
    vec3 at = cam.position + cam.focal_length * direction;
    vec3 from = cam.position + disc;
    
    Ray ray;
    ray.origin = from;
    ray.direction = normalize(at - from);

    return ray;
}

#endif