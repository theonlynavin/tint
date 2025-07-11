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
        /// @brief Constructs an AABB containing given triangles
        /// @param triangles Set of triangles
        AABB(const std::vector<Triangle>& triangles = {});
        ~AABB();

        /// @brief Includes the triangle inside the bounds
        /// @param tri Triangle
        void Expand(const Triangle& tri);

        /// @brief Includes the vertex inside the bounds
        /// @param vertex Vertex
        void Expand(const glm::vec3& vertex);

        /// @brief Includes the other bounds inside via union
        /// @param other Other bounds
        void Expand(const AABB& other);

        /// @brief Tests the ray against the box
        /// @param ray Ray (ray distances may be modified)
        /// @param t Distance (modified if hit)
        /// @return True if the ray hits the object, False otherwise
        bool Intersect(const Ray& ray, float& t) const;

        /// @brief Minimum coordinate of the AABB
        glm::vec3 GetMin() const;

        /// @brief Maximum coordinate of the AABB
        glm::vec3 GetMax() const;
        
        /// @brief Surface area of the AABB
        glm::vec3 GetCentroid() const;
        
        /// @brief Surface area of the AABB
        real GetArea() const;
        
        /// @brief Intersection of two AABBs
        /// @param first First box
        /// @param second Second box
        static AABB Intersect(const AABB& first, const AABB& second);

        /// @brief Intersection of given set of AABBs
        /// @param aabbs Set of AABBs
        static AABB Intersect(const std::vector<AABB>& aabbs);

        /// @brief Union of two AABBs
        /// @param first First box
        /// @param second Second box
        static AABB Union(const AABB& first, const AABB& second);

        /// @brief Union of given set of AABBs
        /// @param aabbs Set of AABBs
        static AABB Union(const std::vector<AABB>& aabbs);
    };
} // namespace Tint
