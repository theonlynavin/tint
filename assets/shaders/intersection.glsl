#ifndef TINT_INTERSECTION
#define TINT_INTERSECTION

#include "bvh.glsl"

// Closest intersection in the scene given a ray
Surface intersect_scene(Ray ray) {
    Surface surface;
    surface.distance = 1e30; // Infinity
    surface.tri_index = -1;
    traverse_bvh(ray, surface);
    return surface;
}

#endif