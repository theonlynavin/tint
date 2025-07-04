#version 430 core

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
	
// Images 
layout(rgba32f, binding = 0) uniform image2D imgOutput;
layout(rgba32f, binding = 1) uniform image2D accumOutput;

// Include other files
#include "data.glsl"
#include "rng.glsl"
#include "camera.glsl"
#include "illum.glsl"

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
    
    // Path traced color
    vec4 sampleColor = vec4(get_illum(camera, uv, rng_state), 1);
    vec4 accumColor = imageLoad(accumOutput, pixel_coords).rgba;
    vec4 result = (accumColor * frame_count + sampleColor) / (frame_count + 1);

    imageStore(accumOutput, pixel_coords, result);
    imageStore(imgOutput, pixel_coords, pow(result, vec4(1/1.5)));
}