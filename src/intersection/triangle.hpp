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

    struct gl_Triangle
    {
        glm::vec4 v0xyz_n0x;  
        glm::vec4 v1xyz_n0y;  
        glm::vec4 v2xyz_n0z;  
        glm::vec4 n1xyz_empty;  
        glm::vec4 n2xyz_material_id;
    };
    
    struct Triangle
    {
        Vertex v1, v2, v3;
        uint materialID;
        bool intersect(Ray &ray, glm::vec2 &uv) const;
        glm::vec3 normal(glm::vec2 uv) const;
        glm::vec3 point(glm::vec2 uv) const;
        real area() const;
        gl_Triangle ToGLTriangle() const;
    };

    struct Surface
    {
        Triangle hit;
        glm::vec2 uv;
    };
} // namespace Tint
