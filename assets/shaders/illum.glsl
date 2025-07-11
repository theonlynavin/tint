#ifndef TINT_ILLUMINATION
#define TINT_ILLUMINATION

#include "sampler.glsl"
#include "intersection.glsl"

// Illumination of a given pixel
vec3 get_illum(Camera camera, vec2 uv, inout uint rng_state) {

    Ray ray = generate_ray(camera, uv, rng_state);

    vec3 color = vec3(0.0);       // Accumulated color
    vec3 throughput = vec3(1.0);    // Energy carried by the ray (starts at full brightness)

    for(int bounce = 0; bounce < 12; bounce++) {
        // Intersect the scene
        Surface hit = intersect_scene(ray);

        // If no hit, return sky color (or background)
        if(hit.tri_index == -1) {
            vec3 skyColor = vec3(0);// mix(vec3(1, 1, 1), vec3(0.5, 0.7, 0.9), uv.y);  // Light blue sky
            color += throughput * skyColor;
            break;
        }

        // Fetch material properties (albedo, emission, etc.)
        vec3 albedo = vec3(0.3, 0.4, 0.5); 
        vec3 emission = vec3(0.5, 0.2, 0.3);

        color += throughput * emission;
        throughput *= albedo;

        // Russian Roulette termination
        float survivalProbability = max(throughput.r, max(throughput.g, throughput.b));
        if(bounce > 3 && survivalProbability < 0.1) {
            if(rng_next(rng_state) > survivalProbability)
                break;
            throughput /= survivalProbability;
        }

        // Bounce the ray (diffuse reflection)
        ray.origin = hit.point + hit.normal * 0.001;  
        ray.direction = normalize(hit.normal + sample_uniform_hemisphere(rng_state));
    }

    return color;
}

#endif