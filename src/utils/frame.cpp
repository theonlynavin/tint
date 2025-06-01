#include "frame.hpp"

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
    frameToWorld = glm::identity<glm::mat4>();
    frameToWorld = glm::scale(frameToWorld, scale);
    frameToWorld = glm::mat4_cast(rotation) * frameToWorld;
    frameToWorld = glm::translate(frameToWorld, position);

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
