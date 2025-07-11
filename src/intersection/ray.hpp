#pragma once
#include "../utils/utils.hpp"

namespace Tint
{
    struct Ray
    {
        /// @brief Initializes the ray
        /// @param origin Origin of the ray
        /// @param direction Must be normalized
        Ray(glm::vec3 origin, glm::vec3 direction)
            : origin(origin), direction(direction)
        {
        }

        glm::vec3 origin;
        glm::vec3 direction;

        float tMin = FLT_EPSILON, tMax = FLT_MAX;

        /// @brief Returns the point a distance t along the ray from the origin
        inline glm::vec3 at(float t) const { return origin + t * direction; }

        /// @brief Returns the point a distance t along the ray from the origin
        inline glm::vec3 operator()(float t) const { return origin + t * direction; } 
    };  
    
} // namespace Tint
