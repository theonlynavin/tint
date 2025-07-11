#include "triangle.hpp"
#include "ray.hpp"

Tint::Triangle::Triangle()
{
}

Tint::Triangle::Triangle(const Vertex &v1, const Vertex &v2, const Vertex &v3)
    : v1(v1), v2(v2), v3(v3), materialID(0)
{
    //TODO: materialID TT
}

bool Tint::Triangle::Intersect(Ray &ray, glm::vec2 &uv) const
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

glm::vec3 Tint::Triangle::GetNormal(glm::vec2 uv) const
{
    return glm::normalize(uv.x * v2.normal + uv.y * v3.normal + (1 - uv.x - uv.y) * v1.normal);
}

glm::vec3 Tint::Triangle::GetPoint(glm::vec2 uv) const
{
    return glm::vec3(uv.x * v2.position + uv.y * v3.position + (1 - uv.x - uv.y) * v1.position);
}

Tint::real Tint::Triangle::GetArea() const
{
    return glm::length(glm::cross(v2.position - v1.position, v3.position - v1.position)) * 0.5;
}

Tint::gl::Triangle Tint::Triangle::ToGLTriangle() const
{
    return gl::Triangle{
        glm::vec4(v1.position, v1.normal.x),
        glm::vec4(v2.position, v1.normal.y),
        glm::vec4(v3.position, v1.normal.z),
        glm::vec4(v2.normal, 0),
        glm::vec4(v3.normal, materialID),
        glm::vec4(v1.texCoords, v2.texCoords),
        glm::vec4(v3.texCoords, 0, 0)
    };
}
