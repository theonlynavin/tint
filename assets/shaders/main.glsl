#version 430 core

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
	
// Images 
layout(rgba32f, binding = 0) uniform image2D imgOutput;

// Include other files
#include "data.glsl"
#include "rng.glsl"
#include "camera.glsl"
#include "intersection.glsl"

uniform Camera camera;
uniform int frame_count;

void main() {
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 image_size = imageSize(imgOutput);
    
    if (pixel_coords.x >= image_size.x || pixel_coords.y >= image_size.y)
        return;
    
    // Initialize RNG
    uint rng_state = hash(pixel_coords.x + pixel_coords.y * image_size.x + frame_count * 123456789);
    
    // Calculate normalized device coordinates [-1, 1]
    vec2 uv = vec2(
        (float(pixel_coords.x) + rng_next(rng_state)) / float(image_size.x),
        (float(pixel_coords.y) + rng_next(rng_state)) / float(image_size.y)
    );
    
    // Generate camera ray
    Ray ray = generate_ray(camera, uv, rng_state);
    
    // Intersect scene
    Surface surface = intersect_scene(ray);
    
    // Output depth as color
    vec4 color;
    if (surface.tri_index >= 0) {
        float depth = surface.distance;
        color = vec4(vec3(depth * 0.1), 1.0); // Scale depth for better visualization
    } else {
        color = vec4(0.0, 0.0, 0.0, 1.0); // Black for no hit
    }

    imageStore(imgOutput, pixel_coords, color);
}