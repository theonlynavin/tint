#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <GL/glew.h>

#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#ifdef TINGE_DEBUG
#define GL_CALL(f) { f; Tint::GLCheckErrors(__FILE__, __LINE__); }
#else
#define GL_CALL(f) { f; }
#endif

namespace Tint
{
    typedef glm::vec2 vec2;
    typedef glm::vec3 vec3;
    typedef glm::vec4 vec4;
    typedef float real;

    typedef glm::mat3 mat3;
    typedef glm::mat4 mat4;

    typedef struct complex { real re, img; } complex;

    void GLInitialize();

    void GLCheckErrors(const char* file, int line);
    
    void TDebugMessage(const std::string& message);

    void TRaiseError(const std::string& message, const std::string& raiser);
    void TRaiseWarning(const std::string& message, const std::string& raiser);
} // namespace Tint
