#pragma once
#include "utils.hpp"

namespace Tint
{    
    class Frame
    {
    public:
        Frame();
        ~Frame();
        
        void Translate(glm::vec3 translation);
        void Rotate(glm::vec3 eulerAngles);
        void Scale(glm::vec3 scale);

        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;
        
        void LockTransform();

        glm::mat4 GetFrameToWorld() const;
        glm::mat4 GetWorldToFrame() const;
    private:
        glm::mat4 frameToWorld, worldToFrame;
    };    

    glm::vec3 toEulerAngles(glm::quat rotation);
    glm::quat toQuaternion(glm::vec3 eulerAngles);
} // namespace Tint
