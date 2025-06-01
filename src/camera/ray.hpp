#include "../utils/utils.hpp"

namespace Tint
{
    struct Ray
    {
        /// @brief Initializes the ray
        /// @param origin Origin of the ray
        /// @param direction Must be normalized
        Ray(glm::vec3 origin, glm::vec3 direction, float wavelength)
            : origin(origin), direction(direction), wavelength(wavelength)
        {
        }

        glm::vec3 origin;
        glm::vec3 direction;
        float wavelength;

        inline glm::vec3 at(float t) const { return origin + t * direction; }
        inline glm::vec3 operator()(float t) const { return origin + t * direction; } 
    };  
    
} // namespace Tint
