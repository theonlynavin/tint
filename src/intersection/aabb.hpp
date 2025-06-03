#pragma once
#include "../utils/utils.hpp"

namespace Tint
{
    struct Triangle;
    struct Ray;

    class AABB
    {
    private:
        glm::vec3 min, max;
    public:
        AABB(const std::vector<Triangle>& triangles = {});
        ~AABB();
        void Expand(const Triangle& tri);
        void Expand(const glm::vec3& vertex);
        void Expand(const AABB& other);
        bool Intersect(const Ray& ray, float& t) const;
        glm::vec3 GetMin() const;
        glm::vec3 GetMax() const;
        glm::vec3 GetCentroid() const;
        real GetArea() const;
        
        static AABB Intersect(const AABB& first, const AABB& second);
        static AABB Intersect(const std::vector<AABB>& aabbs);

        static AABB Union(const AABB& first, const AABB& second);
        static AABB Union(const std::vector<AABB>& aabbs);
    };
} // namespace Tint
