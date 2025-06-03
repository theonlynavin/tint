#include "aabb.hpp"
#include "triangle.hpp"
#include "../camera/ray.hpp"

Tint::AABB::AABB(const std::vector<Triangle> &triangles)
    : min(INFINITY), max(-INFINITY)
{
    for (auto &&tri : triangles)
    {
        Expand(tri);
    }    
}

Tint::AABB::~AABB()
{
}

void Tint::AABB::Expand(const Triangle &tri)
{
    real minX = glm::min(glm::min(tri.v1.position.x, tri.v2.position.x), tri.v3.position.x);
    real minY = glm::min(glm::min(tri.v1.position.y, tri.v2.position.y), tri.v3.position.y);
    real minZ = glm::min(glm::min(tri.v1.position.z, tri.v2.position.z), tri.v3.position.z);

    real maxX = glm::max(glm::max(tri.v1.position.x, tri.v2.position.x), tri.v3.position.x);
    real maxY = glm::max(glm::max(tri.v1.position.y, tri.v2.position.y), tri.v3.position.y);
    real maxZ = glm::max(glm::max(tri.v1.position.z, tri.v2.position.z), tri.v3.position.z);

    min = glm::min(min, glm::vec3(minX, minY, minZ));
    max = glm::max(max, glm::vec3(maxX, maxY, maxZ));
}

void Tint::AABB::Expand(const glm::vec3 &vertex)
{
    min = glm::min(min, vertex);
    max = glm::max(max, vertex);
}

void Tint::AABB::Expand(const AABB &other)
{
    min = glm::min(min, other.min);
    max = glm::max(max, other.max);
}

/// http://psgraphics.blogspot.com/2016/02/new-simple-ray-box-test-from-andrew.html
bool Tint::AABB::Intersect(const Ray &ray, float &t) const
{
    float tmin = FLT_EPSILON, tmax = FLT_MAX;

    for (int a = 0; a < 3; ++a) 
    {
        float invD = 1.0f / ray.direction[a];
        float t0 = (min[a] - ray.origin[a]) * invD;
        float t1 = (max[a] - ray.origin[a]) * invD;
        
        if (invD < 0.0f) {
            float temp = t1;
            t1 = t0;
            t0 = temp;
        }

        tmin = t0 > tmin ? t0 : tmin;
        tmax = t1 < tmax ? t1 : tmax;

        if (tmax < tmin)
            return false;
    }

    if (tmin > ray.tMin && tmin < ray.tMax)
    {
        t = tmin;
        return true;
    }
    else if (tmax > ray.tMin && tmax < ray.tMax)
    {
        t = tmax;
        return true;
    }

    return false;
}

glm::vec3 Tint::AABB::GetMin() const
{
    return min;
}

glm::vec3 Tint::AABB::GetMax() const
{
    return max;
}

glm::vec3 Tint::AABB::GetCentroid() const
{
    return 0.5f * (min + max);
}

Tint::real Tint::AABB::GetArea() const
{
    real a = (max.x - min.x);
    real b = (max.y - min.y);
    real c = (max.z - min.z);
    return 2*(a*b + b*c + c*a);
}

Tint::AABB Tint::AABB::Intersect(const AABB &first, const AABB &second)
{
    AABB result;
    result.min = glm::max(first.min, second.min);
    result.max = glm::min(first.max, second.max);

    return result;
}

Tint::AABB Tint::AABB::Intersect(const std::vector<AABB> &aabbs)
{
    AABB result;
    
    for (size_t i = 0; i < aabbs.size(); i++)
        result = Intersect(result, aabbs[i]);

    return result;
}

Tint::AABB Tint::AABB::Union(const AABB &first, const AABB &second)
{
    AABB result;
    result.min = glm::min(first.min, second.min);
    result.max = glm::max(first.max, second.max);

    return result;
}

Tint::AABB Tint::AABB::Union(const std::vector<AABB> &aabbs)
{
    AABB result;

    for (size_t i = 0; i < aabbs.size(); i++)
        result = Union(result, aabbs[i]);

    return result;
}
