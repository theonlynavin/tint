#include "triangle.hpp"
#include "../camera/ray.hpp"

bool Tint::Triangle::intersect(const Ray &ray, glm::vec2 &uv, float &t) const
{
    glm::vec3 e21 = v2.position - v1.position;
    glm::vec3 e31 = v3.position - v1.position;
    glm::vec3 ray_cross_e32 = cross(ray.direction, e31);
    float det = dot(e21, ray_cross_e32);

    if (det > -FLT_EPSILON && det < FLT_EPSILON)
        return false;

    float inv_det = 1.0 / det;
    glm::vec3 s = ray.origin - v1.position;
    float u = inv_det * dot(s, ray_cross_e32);

    if ((u < 0 && abs(u) > FLT_EPSILON) || (u > 1 && abs(u-1) > FLT_EPSILON))
        return {};

    glm::vec3 s_cross_e21 = cross(s, e21);
    float v = inv_det * dot(ray.direction, s_cross_e21);

    if ((v < 0 && abs(v) > FLT_EPSILON) || (u + v > 1 && abs(u + v - 1) > FLT_EPSILON))
        return false;

    t = inv_det * dot(e31, s_cross_e21);

    if (t > FLT_EPSILON)
    {
        uv.x = u;
        uv.y = v;
        return true;
    }

    return false;
}

glm::vec3 Tint::Triangle::normal(glm::vec2 uv) const
{
    return glm::normalize(uv.x * v1.normal + uv.y * v2.normal + (1 - uv.x - uv.y) * v3.normal);
}
