#pragma once
#include "../utils/utils.hpp"
#include "../intersection/ray.hpp"
#include "frame.hpp"

namespace Tint
{
    struct RandomState;

    namespace gl
    {
        struct Camera
        {
            glm::vec3 fwd, up, right;
            glm::vec3 position;
            float focal_length;
            float aperture;
            float aspect;
            float scale;
        };
    }

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

        gl::Camera ToGLCamera() const;
        
        float aperture;
        float focalLength;
        float fieldOfView;
        glm::vec2 filmSize;

        Frame frame;
    };
} // namespace Tint
