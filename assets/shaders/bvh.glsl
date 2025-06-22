bool intersect_aabb(Ray ray, vec3 aabb_min, vec3 aabb_max, out float t) {
    vec3 inv_dir = 1.0 / ray.direction;
    
    // Calculate intersections with the AABB's slabs
    vec3 t0 = (aabb_min - ray.origin) * inv_dir;
    vec3 t1 = (aabb_max - ray.origin) * inv_dir;
    
    // For each axis, find the near and far intersection distances
    vec3 tmin = min(t0, t1);
    vec3 tmax = max(t0, t1);
    
    // Find the largest tmin and smallest tmax
    float t_entry = max(max(tmin.x, tmin.y), tmin.z);
    float t_exit = min(min(tmax.x, tmax.y), tmax.z);
    
    // Check if intersection is valid
    if (t_entry > t_exit) {
        return false;
    }
    
    // Return the closest intersection distance
    t = (t_entry > 0.0) ? t_entry : t_exit;
    return true;
}

#include "triangle.glsl"

void traverse_bvh(Ray ray, inout Surface surface) {
    
    int stack[32];
    int stack_ptr = 0;
    stack[stack_ptr++] = 0; // Push root node
    
    while (stack_ptr > 0) {
        
        int node_idx = stack[--stack_ptr];
        BVHNode node = nodes[node_idx];
        vec4 node_min = node.bounds_min;//  bounds_min[node_idx];
        vec4 node_max = node.bounds_max;// bounds_max[node_idx];
        
        float t_aabb = -1;
        bool hit = intersect_aabb(ray, node_min.xyz, node_max.xyz, t_aabb);

        if (hit && t_aabb < surface.distance)
        {                            
            ivec2 data = node.children_or_tris;
            int leaf = int(node_min.w);

            if (leaf == 0) { // Interior node
                stack[stack_ptr++] = data.y; // child_second
                stack[stack_ptr++] = data.x; // child_first
            } else { // Leaf node
                int tri_offset = data.x;
                int tri_count = data.y;

                for (int i = 0; i < tri_count; i++) {
                    int tri_idx = tri_offset + i;
                    Triangle tri = tris[tri_idx];
                    
                    vec3 vert0 = tri.v0.xyz;
                    vec3 vert1 = tri.v1.xyz;
                    vec3 vert2 = tri.v2.xyz;
                    
                    float t, u, v;
                    if (intersect_triangle(ray, vert0, vert1, vert2, t, u, v) && t < surface.distance) {
                        surface.distance = t;
                        surface.tri_index = tri_idx;
                        surface.point = ray.origin + ray.direction * t;
                        
                        // Interpolate normal
                        //vec3 n0 = vec3(tri.v0.w, tri.v1.w, tri.v2.w);
                        //vec3 n1 = tri.n1.xyz;
                        //vec3 n2 = vec3(tri.n1.w, tri.n2.yz);
                        //surface.normal = normalize(mix(mix(n0, n1, u), n2, v));
                    }
                }
            }

            if (stack_ptr >= 32)
                return;
        }
    }
}

uniform int total_tris;

void traverse_triangles(Ray ray, inout Surface surface) {

    for (int i = 0; i < total_tris; i++) {
        Triangle tri = tris[i];
        
        vec3 vert0 = tri.v0.xyz;
        vec3 vert1 = tri.v1.xyz;
        vec3 vert2 = tri.v2.xyz;
        
        float t, u, v;
        if (intersect_triangle(ray, vert0, vert1, vert2, t, u, v) && t < surface.distance) {
            surface.distance = t;
            surface.tri_index = i;
            surface.point = ray.origin + ray.direction * t;
            
            // Interpolate normal
            //vec3 n0 = vec3(tri.v0.w, tri.v1.w, tri.v2.w);
            //vec3 n1 = tri.n1.xyz;
            //vec3 n2 = vec3(tri.n1.w, tri.n2.yz);
            //surface.normal = normalize(mix(mix(n0, n1, u), n2, v));
        }
    }
}