#include "../utils/utils.hpp"
#include "../utils/frame.hpp"
#include "ray.hpp"

namespace Tint
{
    class Camera
    {
    public:
        Camera(glm::vec2 filmSize, float fieldOfView, float focalLength = 0, float aperture = 0);
        ~Camera();
        Ray GenerateRay(float u, float v) const;
        void LookAt(glm::vec3 from, glm::vec3 at);
        float aperture;
        float focalLength;
        float fieldOfView;
        glm::vec2 filmSize;
        Frame frame;
    };
} // namespace Tint
