#include "triangle.hpp"
#include "../camera/ray.hpp"

bool Tint::Triangle::intersect(Ray &ray, glm::vec2 &uv) const
{
    glm::vec3 e21 = v2.position - v1.position;
    glm::vec3 e31 = v3.position - v1.position;
    glm::vec3 pvec = cross(ray.direction, e31);
    float det = dot(e21, pvec);

    if (det < FLT_EPSILON)
        return false;

    float inv_det = 1.0 / det;
    glm::vec3 tvec = ray.origin - v1.position;
    float u = inv_det * dot(tvec, pvec);

    if (u < 0 || u > 1)
        return false;

    glm::vec3 qvec = cross(tvec, e21);
    float v = inv_det * dot(ray.direction, qvec);

    if (v < 0 || u + v > 1)
        return false;

    float t = inv_det * dot(e31, qvec);

    if (t > ray.tMin && t < ray.tMax)
    {
        ray.tMax = t;
        uv.x = u;
        uv.y = v;
        return true;
    }

    return false;
}

glm::vec3 Tint::Triangle::normal(glm::vec2 uv) const
{
    return glm::normalize(uv.x * v2.normal + uv.y * v3.normal + (1 - uv.x - uv.y) * v1.normal);
}

glm::vec3 Tint::Triangle::point(glm::vec2 uv) const
{
    return glm::vec3(uv.x * v2.position + uv.y * v3.position + (1 - uv.x - uv.y) * v1.position);
}

Tint::real Tint::Triangle::area() const
{
    return glm::length(glm::cross(v2.position - v1.position, v3.position - v1.position)) * 0.5;
}

Tint::gl_Triangle Tint::Triangle::ToGLTriangle() const
{
    return gl_Triangle{
        glm::vec4(v1.position, v1.normal.x),
        glm::vec4(v2.position, v1.normal.y),
        glm::vec4(v3.position, v1.normal.z),
        glm::vec4(v2.normal, 0),
        glm::vec4(v3.normal, materialID)
    };
}
