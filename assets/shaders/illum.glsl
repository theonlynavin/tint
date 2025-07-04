#include "sampler.glsl"
#include "intersection.glsl"

vec3 get_illum(Camera camera, vec2 uv, inout uint rng_state) {
    
    Ray ray = generate_ray(camera, uv, rng_state);

    vec3 color = vec3(0.0);         // Accumulated color
    vec3 throughput = vec3(1.0);    // Energy carried by the ray (starts at full brightness)
    
    for (int bounce = 0; bounce < 4; bounce++) {
        // Intersect the scene
        Surface hit = intersect_scene(ray);
        
        // If no hit, return sky color (or background)
        if (hit.tri_index == -1) {
            vec3 skyColor = mix(vec3(1, 1, 1), vec3(0.5, 0.7, 0.9), uv.y);  // Light blue sky
            color += throughput * skyColor;
            break;
        }
        
        // Fetch material properties (albedo, emission, etc.)
        vec3 albedo = vec3(0.8); //fetchMaterialAlbedo(hit.tri_index);  // Assume this function exists
        vec3 emission = vec3(0);//fetchMaterialEmission(hit.tri_index);  // Light-emitting materials
        
        // Add emission (if material is a light source)
        color += throughput * emission;
        
        // Update throughput with albedo attenuation
        throughput *= albedo;
        
        // Russian Roulette termination (early exit for low-energy rays)
        float survivalProbability = max(throughput.r, max(throughput.g, throughput.b));
        if (bounce > 3 && survivalProbability < 0.1) {
            if (rng_next(rng_state) > survivalProbability)  // Assume `rand()` generates [0,1)
                break;
            throughput /= survivalProbability;  // Energy compensation
        }
        
        // Bounce the ray (diffuse reflection)
        ray.origin = hit.point + hit.normal * 0.001;  // Offset to avoid self-intersection
        ray.direction = normalize(hit.normal + sample_uniform_hemisphere(rng_state));  // Diffuse bounce
    }
    
    return color;
}