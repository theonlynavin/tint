#include "camera.hpp"
#include <glm/gtx/string_cast.hpp>
#include "../sampler/random.hpp"

Tint::Camera::Camera(glm::vec2 filmSize, float fieldOfView, float focalLength, float aperture)
    : filmSize(filmSize), fieldOfView(fieldOfView), focalLength(focalLength), aperture(aperture)
{
}

Tint::Camera::~Camera()
{
}

Tint::Ray Tint::Camera::GenerateRay(float u, float v, RandomState& state) const
{
    glm::mat4 cameraToWorld = frame.GetFrameToWorld();

    float aspect = filmSize.y / filmSize.x;
    float scale = tan(fieldOfView / 2);
    float x = (2 * u - 1) * scale;
    float y = (2 * v - 1) * scale * aspect;

    glm::vec3 origin = (cameraToWorld * glm::vec4(0,0,0,1)).xyz();
    glm::vec3 direction = glm::normalize((cameraToWorld * glm::vec4(x,y,-1,0)).xyz());

    // Simple lens defocus
    glm::vec3 defocus = glm::vec3(Random::Uniform2D(state) * aperture, 0);
    glm::vec3 at = origin + focalLength * direction;
    glm::vec3 from = origin + defocus;

    //float wavelength = Sampler::sampleWavelength();

    return Ray(from, glm::normalize(at - from));
}

void Tint::Camera::LookAt(glm::vec3 from, glm::vec3 at)
{
    glm::vec3 dir = from - at;
    focalLength = glm::length(dir);
    frame.position = from;

    glm::vec3 fwd = dir / focalLength;
    glm::vec3 up = glm::vec3(0, 1, 0);

    if (glm::dot(fwd, up) > 1 - FLT_EPSILON || glm::dot(fwd, up) < -1 + FLT_EPSILON)
        up = glm::vec3(0, 0, 1);

    glm::vec3 right = glm::normalize(glm::cross(up, fwd));
    up = glm::cross(fwd, right);

    frame.rotation = glm::quat(glm::mat3(right, up, fwd));
}

Tint::cl_Camera Tint::Camera::ToCLCamera() const
{
    cl_Camera cam;
    
    glm::mat4 cameraToWorld = frame.GetFrameToWorld();
    glm::vec3 right = (cameraToWorld * glm::vec4(1, 0, 0, 0)).xyz();
    glm::vec3 up = (cameraToWorld * glm::vec4(0, 1, 0, 0)).xyz();
    glm::vec3 fwd = (cameraToWorld * glm::vec4(0, 0, 1, 0)).xyz();
    glm::vec3 origin = (cameraToWorld * glm::vec4(0, 0, 0, 1)).xyz();
    cam.fwd = cl_float3{fwd.x, fwd.y, fwd.z};
    cam.up = cl_float3{up.x, up.y, up.z};
    cam.right = cl_float3{right.x, right.y, right.z};
    cam.origin = cl_float3{origin.x, origin.y, origin.z};

    cam.aperture = aperture;
    cam.focal_length = focalLength;
    cam.scale = tan(fieldOfView / 2);
    cam.aspect = filmSize.y / filmSize.x;

    return cam;
}
