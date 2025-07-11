#pragma once
#include "../utils/utils.hpp"

namespace Tint
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
    };    

    struct Ray;

    namespace gl 
    {
        struct Triangle
        {
            glm::vec4 v0xyz_n0x;  
            glm::vec4 v1xyz_n0y;  
            glm::vec4 v2xyz_n0z;  
            glm::vec4 n1xyz_empty;  
            glm::vec4 n2xyz_material_id;
            glm::vec4 t0t1;  
            glm::vec4 t2_empty;
        };
    }
    
    class Triangle
    {
    public:
        Triangle();
        Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3);

        ~Triangle() = default;
        
        /// @brief Moller-Trumbore ray-triangle intersection
        /// @param ray Ray (modified if intersects)
        /// @param uv Barycentric coordinates (modified if intersects)
        /// @return True if intersects, False otherwise
        bool Intersect(Ray &ray, glm::vec2 &uv) const;

        /// @brief Normal at given local coordinates
        /// @param uv Barycentric coordinates
        glm::vec3 GetNormal(glm::vec2 uv) const;

        /// @brief Global position of given local coordinates
        /// @param uv Barycentric coordinates
        glm::vec3 GetPoint(glm::vec2 uv) const;

        /// @brief Surface area of the triangle
        real GetArea() const;

        gl::Triangle ToGLTriangle() const;
        
        Vertex v1, v2, v3;

        uint materialID;
    };

    struct Surface
    {
        Triangle hit;
        glm::vec2 uv;
    };
} // namespace Tint
