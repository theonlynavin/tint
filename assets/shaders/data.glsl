struct BVHNode {
    vec4 bounds_min;  // .w contains node type (0=interior, 1=leaf)
    vec4 bounds_max;  // .w unused
    ivec2 children_or_tris; // For interior: (child_first, child_second)
                            // For leaf: (tri_offset, tri_count)
};

struct Triangle {
    vec4 v0;  // xyz=pos0, w=normal0.x
    vec4 v1;  // xyz=pos1, w=normal0.y
    vec4 v2;  // xyz=pos2, w=normal0.z
    vec4 n0;
    vec4 n1;  // xyz=normal1, w=normal2.x
    vec4 n2;  // yz=normal2.yz, w=material_id
};

// BVH Node Components (separate buffers)
layout(std430, binding = 1) readonly buffer BVHNodes {
    BVHNode nodes[];
};

// Triangle Components (separate buffers)
layout(std430, binding = 2) readonly buffer TriangleData {
    Triangle tris[];
};
