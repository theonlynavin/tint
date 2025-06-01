#include "camera.hpp"

Tint::Camera::Camera(glm::vec2 filmSize, float fieldOfView, float focalLength, float aperture)
    : filmSize(filmSize), fieldOfView(fieldOfView), focalLength(focalLength), aperture(aperture)
{
}

Tint::Camera::~Camera()
{
}

Tint::Ray Tint::Camera::GenerateRay(float u, float v) const
{
    glm::mat4 cameraToWorld = frame.GetFrameToWorld();
    glm::vec3 origin = (cameraToWorld * glm::vec4(0,0,0,1)).xyz();
    glm::vec3 direction = glm::normalize((cameraToWorld * glm::vec4(u, v, -1, 0)).xyz());

    float wavelength = Sampler::sampleWavelength();

    return Ray(origin, direction);
}
