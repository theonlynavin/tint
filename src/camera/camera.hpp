#include "../utils/utils.hpp"
#include "../utils/frame.hpp"
#include "ray.hpp"

namespace Tint
{
    struct RandomState;

    class Camera
    {
    public:
        /// @brief 
        /// @param filmSize 
        /// @param fieldOfView 
        /// @param focalLength Must be strictly non-zero
        /// @param aperture 
        Camera(glm::vec2 filmSize, float fieldOfView, float focalLength = 1, float aperture = 0);
        ~Camera();

        Ray GenerateRay(float u, float v, RandomState& state) const;

        /// @brief Orients the camera and adjusts its focal length
        /// @param from Places the camera frame here
        /// @param at The camera looks towards here
        void LookAt(glm::vec3 from, glm::vec3 at);
        
        float aperture;
        float focalLength;
        float fieldOfView;
        glm::vec2 filmSize;

        Frame frame;
    };
} // namespace Tint
