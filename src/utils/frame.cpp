#include "frame.hpp"
#include <glm/gtx/string_cast.hpp>

Tint::Frame::Frame(glm::vec3 position, glm::quat rotation, glm::vec3 scale)
    : position(position), rotation(rotation), scale(scale)
{
}

Tint::Frame::~Frame()
{
}

void Tint::Frame::Translate(glm::vec3 translation)
{
    position += translation;
}

void Tint::Frame::Scale(glm::vec3 scale)
{
    this->scale *= scale;
}

void Tint::Frame::Rotate(glm::vec3 eulerAngles)
{
    rotation = glm::quat(eulerAngles) * rotation;
}

void Tint::Frame::LockTransform()
{
    glm::mat4 T = glm::translate(glm::mat4(1), position);
    glm::mat4 R = glm::mat4_cast(rotation);
    glm::mat4 S = glm::scale(glm::mat4(1), scale);

    frameToWorld = T * R * S;

    if (parent != NULL)
    {
        parent->LockTransform();
        frameToWorld = parent->GetFrameToWorld() * frameToWorld;
    }

    worldToFrame = glm::inverse(frameToWorld);
}


glm::mat4 Tint::Frame::GetFrameToWorld() const
{
    return frameToWorld;
}

glm::mat4 Tint::Frame::GetWorldToFrame() const
{
    return worldToFrame;
}
