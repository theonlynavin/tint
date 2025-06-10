#pragma once

#include <CL/opencl.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Tint
{
    typedef glm::vec2 vec2;
    typedef glm::vec3 vec3;
    typedef glm::vec4 vec4;
    typedef float real;

    typedef glm::mat3 mat3;
    typedef glm::mat4 mat4;

    typedef struct complex { real re, img; } complex;
} // namespace Tint
