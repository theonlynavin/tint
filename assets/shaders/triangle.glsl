struct Surface {
    int tri_index;
    vec3 normal;
    vec3 point;
    float distance;
};

bool intersect_triangle(Ray ray, vec3 v0, vec3 v1, vec3 v2, out float t, out float u, out float v) {
    const float EPSILON = 0.0000001;
    vec3 edge1 = v1 - v0;
    vec3 edge2 = v2 - v0;
    vec3 h = cross(ray.direction, edge2);
    float a = dot(edge1, h);
    
    if (a > -EPSILON && a < EPSILON)
        return false;
    
    float f = 1.0 / a;
    vec3 s = ray.origin - v0;
    u = f * dot(s, h);
    
    if (u < 0.0 || u > 1.0)
        return false;
    
    vec3 q = cross(s, edge1);
    v = f * dot(ray.direction, q);
    
    if (v < 0.0 || u + v > 1.0)
        return false;
    
    t = f * dot(edge2, q);
    return t > EPSILON;
}
