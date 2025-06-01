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
        bool intersect(const Ray &ray, glm::vec2 &uv, float &t) const;
        glm::vec3 normal(glm::vec2 uv) const;
    };    
} // namespace Tint
