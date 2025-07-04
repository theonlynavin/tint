#include "bvh.glsl"

Surface intersect_scene(Ray ray) {
    Surface surface;
    surface.distance = 1e30; // Infinity
    surface.tri_index = -1;
    
    traverse_bvh(ray, surface);

    return surface;
}