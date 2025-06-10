#ifndef TINT_BVH
#define TINT_BVH

#include "ray.cl"
#include "triangle.cl"

typedef struct
{
    float3 aabb_min;
    float3 aabb_max;
    
    int first_child;
    int second_child;
    int tri_offset;
    int tri_count;

    int is_leaf;
    int pad0, pad1, pad2; // padding for alignment
} BVHNode;

float intersect_aabb(float3 aabb_min, float3 aabb_max, Ray ray)
{
    float3 invD = 1.0f / ray.direction;
    float3 t0 = (aabb_min - ray.origin) * invD;
    float3 t1 = (aabb_max - ray.origin) * invD;
    
    float3 tmin3 = fmin(t0, t1);
    float3 tmax3 = fmax(t0, t1);

    float tmin = fmax(fmax(tmin3.x, tmin3.y), fmax(tmin3.z, 0.0f));
    float tmax = fmin(fmin(tmax3.x, tmax3.y), tmax3.z);

    return tmax >= tmin ? tmin : -1.0f;
}

typedef struct {
    float3 point;        // .xyz = hit position, .w unused
    float3 normal;       // .xyz = surface normal, .w unused
    float distance;
    int material_id;
    int hit_anything; 
    int pad0;
} Surface;

Surface intersect_bvh(
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
    __global float3* restrict n2s,  Ray ray) {

    Surface hit;
    hit.hit_anything = false;

    int closest_triangle = -1;
    float2 closest_uv = (float2)(0, 0);
    float closest_dist = FLT_MAX;

    int visit_stack[16];
    visit_stack[0] = 0;
    int to_visit = 1;

    while (to_visit > 0) {

        int bvh_index = visit_stack[--to_visit];

        float t_aabb = intersect_aabb(aabb_min[bvh_index], aabb_max[bvh_index], ray); 
        if (t_aabb < 0 || t_aabb >= closest_dist) continue;
        //{ closest_dist = 1; closest_triangle = 0;}

        if (is_leaf[bvh_index]) {
            
            int cur_tri_offset = tri_offset[bvh_index];
            int cur_tri_count = tri_count[bvh_index];

            for (size_t i = cur_tri_offset; i < cur_tri_offset + cur_tri_count; i++) {

                float3 tuv_tri = intersect_triangle(v0s[i], v1s[i], v2s[i], ray);
                float t_tri = tuv_tri.x;

                if (t_tri > 0 && t_tri < closest_dist) {
                    closest_dist = t_tri;
                    closest_uv.x = tuv_tri.y;
                    closest_uv.y = tuv_tri.z;
                    closest_triangle = i;
                }
            }
            
            continue;
        }
        else {

            visit_stack[to_visit++] = second_child[bvh_index];
            visit_stack[to_visit++] = first_child[bvh_index];
            /*BVHNode first = bvh[current_node.first_child];
            BVHNode second = bvh[current_node.second_child];
            
            float t0 = intersect_aabb(first.aabb_min, first.aabb_max, ray); 
            float t1 = intersect_aabb(second.aabb_min, second.aabb_max, ray); 
            bool h0 = t0 > 0 && t0 < closest_dist;
            bool h1 = t1 > 0 && t1 < closest_dist;

            if (h0 && h1) {
                if (t0 < t1) {
                    visit_stack[to_visit++] = current_node.second_child;
                    visit_stack[to_visit++] = current_node.first_child;
                }
                else {
                    visit_stack[to_visit++] = current_node.first_child;
                    visit_stack[to_visit++] = current_node.second_child;
                }
            }
            else if (h0) {
                visit_stack[to_visit++] = current_node.first_child;
            }
            else if (h1) {
                visit_stack[to_visit++] = current_node.second_child;
            }*/
        }

        if (to_visit > 15)
            return hit;
    }

    if (closest_triangle >= 0) {
        hit.point = get_triangle_point(v0s[closest_triangle], v1s[closest_triangle], v2s[closest_triangle], closest_uv);
        hit.normal = get_triangle_normal(n0s[closest_triangle], n1s[closest_triangle], n2s[closest_triangle], closest_uv);
        hit.distance = closest_dist;
        //hit.material_id = closest.material_id;
        hit.hit_anything = true;
    }

    return hit;
}

Surface intersect_triangles(
    __global float3* restrict v0s, 
    __global float3* restrict v1s, 
    __global float3* restrict v2s, 
    __global float3* restrict n0s, 
    __global float3* restrict n1s, 
    __global float3* restrict n2s, int num_triangles, Ray ray) {

    Surface hit;
    hit.hit_anything = false;

    int closest_triangle = -1;
    float2 closest_uv = (float2)(0, 0);
    float closest_dist = FLT_MAX;

    for (int i = 0; i < num_triangles; i++) {
        float3 tuv_tri = intersect_triangle(v0s[i], v1s[i], v2s[i], ray);
        float t_tri = tuv_tri.x;

        if (t_tri > 0 && t_tri < closest_dist) {
            closest_dist = t_tri;
            closest_uv.x = tuv_tri.y;
            closest_uv.y = tuv_tri.z;
            closest_triangle = i;
        }
    }

    if (closest_triangle >= 0) {
        hit.point = get_triangle_point(v0s[closest_triangle], v1s[closest_triangle], v2s[closest_triangle], closest_uv);
        hit.normal = get_triangle_normal(n0s[closest_triangle], n1s[closest_triangle], n2s[closest_triangle], closest_uv);
        hit.distance = closest_dist;
        //hit.material_id = closest.material_id;
        hit.hit_anything = true;
    }

    return hit;
}

#endif  