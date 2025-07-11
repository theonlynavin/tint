#pragma once
#include "../utils/utils.hpp"

namespace Tint
{
    class Frame
    {
    public:
        Frame(glm::vec3 position = glm::vec3(0,0,0), glm::quat rotation = glm::quat(1,0,0,0), glm::vec3 scale=glm::vec3(1,1,1));
        ~Frame();
        
        /// @brief Moves the origin of the frame
        /// @param translation 
        void Translate(glm::vec3 translation);

        /// @brief Rotates the frame about the global origin
        /// @param eulerAngles 
        void Rotate(glm::vec3 eulerAngles);

        /// @brief Scales the frame
        /// @param scale 
        void Scale(glm::vec3 scale);

        std::shared_ptr<Frame> parent;  // Frame coordinates are with respect to the parent frame, if NULL then world frame
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;
        
        /// @brief Locks the frame and computes transformation matrices
        /// @note Calling LockTransform will recursively call the same on parent frames as well
        void LockTransform();

        glm::mat4 GetFrameToWorld() const;
        glm::mat4 GetWorldToFrame() const;
    private:
        glm::mat4 frameToWorld, worldToFrame;
    };   
} // namespace Tint
