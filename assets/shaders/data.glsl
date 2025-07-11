#ifndef TINT_DATA
#define TINT_DATA

layout(binding = 2) uniform samplerBuffer bvhNodesTex;
layout(binding = 3) uniform samplerBuffer trianglesTex;

struct BVHNode {
    vec4 bounds_min;  // .w contains node type (0=interior, 1=leaf)
    vec4 bounds_max;  // .w unused
    ivec2 data;       // For interior: (child_first, child_second)
                      // For leaf: (tri_offset, tri_count)
};

struct Triangle {
    vec3 v0; 
    vec3 v1;  
    vec3 v2;  
    vec3 n0;
    vec3 n1;  
    vec3 n2;  
    vec2 t0;  
    vec2 t1;  
    vec2 t2;
    int material_id;
};

BVHNode fetch_bvh_node(int index) {
    BVHNode node;
    node.bounds_min = texelFetch(bvhNodesTex, index * 3 + 0);
    node.bounds_max = texelFetch(bvhNodesTex, index * 3 + 1);
    vec4 temp = texelFetch(bvhNodesTex, index * 3 + 2);
    node.data = ivec2(temp.x, temp.y);
    return node;
}

Triangle fetch_triangle(int index) {
    Triangle tri;
    vec4 v0 = texelFetch(trianglesTex, index * 7 + 0);
    vec4 v1 = texelFetch(trianglesTex, index * 7 + 1);
    vec4 v2 = texelFetch(trianglesTex, index * 7 + 2);
    vec4 n1 = texelFetch(trianglesTex, index * 7 + 3);
    vec4 n2 = texelFetch(trianglesTex, index * 7 + 4);
    vec4 t0 = texelFetch(trianglesTex, index * 7 + 5);
    vec4 t1 = texelFetch(trianglesTex, index * 7 + 6);
    tri.v0 = v0.xyz;
    tri.v1 = v1.xyz;
    tri.v2 = v2.xyz;
    tri.n0 = vec3(v0.w, v1.w, v2.w);
    tri.n1 = n1.xyz;
    tri.n2 = n2.xyz;
    tri.material_id = int(n2.w);
    tri.t0 = t0.xy;
    tri.t1 = t0.zw;
    tri.t2 = t1.xy;
    return tri;
}

#endif