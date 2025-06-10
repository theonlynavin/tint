#ifndef TINT_INTERSECTION
#define TINT_INTERSECTION

#include "bvh.cl"
#include "random.cl"
#include "camera.cl"

kernel void intersect_scene(Camera cam, 
    __global float3* restrict aabb_min,
    __global float3* restrict aabb_max,
    __global int* restrict first_child,
    __global int* restrict second_child,
    __global int* restrict tri_offset,
    __global int* restrict tri_count,
    __global int* restrict is_leaf,
    __global float3* restrict v0s, 
    __global float3* restrict v1s, 
    __global float3* restrict v2s, 
    __global float3* restrict n0s, 
    __global float3* restrict n1s, 
    __global float3* restrict n2s, 
    const int num_tris,
    const int film_width, const int film_height, 
    __global float* depth_buffer) {
        
    int x = get_global_id(0);
    int y = get_global_id(1);

    if (x >= film_width || y >= film_height) return;

    float u = (float)x / (float)(film_width - 1);
    float v = 1 - (float)y / (float)(film_height - 1);

    int pixel_index = y * film_width + x;

    uint base_seed = 12345u; // fixed seed for reproducibility
    uint pixel_seed = base_seed + pixel_index;

    PCG32State rng;
    rng.state = 0u;
    rng.inc = (pixel_index << 1u) | 1u;  // unique odd increment per pixel
    
    Ray ray = create_camera_ray(cam, u, v, &rng);
    Surface hit = intersect_bvh(aabb_min, aabb_max, first_child, second_child, tri_offset, tri_count, is_leaf, v0s, v1s, v2s, n0s, n1s, n2s, ray);
    //Surface hit = intersect_triangles(v0s, v1s, v2s, n0s, n1s, n2s, num_tris, ray);

    // Flattened buffer index
    depth_buffer[pixel_index] = hit.hit_anything ? exp(-sqrt(hit.distance)) : (0);
}

#endif
