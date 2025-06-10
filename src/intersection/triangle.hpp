#pragma once
#include "../utils/utils.hpp"

namespace Tint
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
    };    

    struct Ray;
    
    struct Triangle
    {
        Vertex v1, v2, v3;
        bool intersect(Ray &ray, glm::vec2 &uv) const;
        glm::vec3 normal(glm::vec2 uv) const;
        glm::vec3 point(glm::vec2 uv) const;
        real area() const;
    };

    struct cl_Triangle
    {
        cl_float3 v0, v1, v2;
        cl_float3 n0, n1, n2;
        //int material_id;
    };

    struct Surface
    {
        Triangle hit;
        glm::vec2 uv;
    };
} // namespace Tint
