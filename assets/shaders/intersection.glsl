
struct Surface {
    int tri_index;
    vec3 normal;
    vec3 point;
    float distance;
};

#include "bvh.glsl"

Surface intersect_scene(Ray ray) {
    Surface surface;
    surface.distance = 10000000.0; // Infinity
    surface.tri_index = -1;
    
    traverse_triangles(ray, surface);
    return surface;
}