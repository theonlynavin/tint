#ifndef TINT_BVH
#define TINT_BVH

bool intersect_aabb(Ray ray, vec3 aabb_min, vec3 aabb_max, out float t) {
    vec3 inv_dir = 1.0 / ray.direction;

    vec3 t0 = (aabb_min - ray.origin) * inv_dir;
    vec3 t1 = (aabb_max - ray.origin) * inv_dir;

    // For each axis, find the near and far intersection distances
    vec3 tmin = min(t0, t1);
    vec3 tmax = max(t0, t1);

    // Find the largest tmin and smallest tmax
    float t_entry = max(max(tmin.x, tmin.y), tmin.z);
    float t_exit = min(min(tmax.x, tmax.y), tmax.z);

    if(t_entry > t_exit) {
        return false;
    }

    t = (t_entry > 0.0) ? t_entry : t_exit;
    return true;
}

#include "triangle.glsl"

void traverse_bvh(Ray ray, inout Surface surface) {
    
    // A stack based BVH traversal
    // The CPU construction ensures that a depth of 32 is never hit
    int stack[32];
    int stack_ptr = 0;
    stack[stack_ptr++] = 0;

    while(stack_ptr > 0) {
        int node_idx = stack[--stack_ptr];
        BVHNode node = fetch_bvh_node(node_idx);

        float t_aabb;
        bool hit = intersect_aabb(ray, node.bounds_min.xyz, node.bounds_max.xyz, t_aabb);

        if(hit && t_aabb < surface.distance) {
            bool leaf = (node.bounds_min.w > 0.5);

            if(leaf == false) { // Interior node
                stack[stack_ptr++] = node.data.y; // child_second
                stack[stack_ptr++] = node.data.x; // child_first
            } else { // Leaf node
                int tri_offset = node.data.x;
                int tri_count = node.data.y;

                for(int i = 0; i < tri_count; i++) {
                    int tri_idx = tri_offset + i;
                    Triangle tri = fetch_triangle(tri_idx);

                    float t, u, v;
                    if(intersect_triangle(ray, tri.v0, tri.v1, tri.v2, t, u, v)) {
                        surface.distance = t;
                        surface.tri_index = tri_idx;
                        surface.point = ray.origin + ray.direction * t;
                        surface.normal = normalize(mix(mix(tri.n0, tri.n1, u), tri.n2, v));
                    }
                }
            }
        }
    }
}

#endif