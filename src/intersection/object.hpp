#include "../utils/frame.hpp"
#include "triangle.hpp"
#include <vector>
#include <string>

namespace Tint
{
    struct Ray;

    struct Surface
    {
        Triangle hit;
        glm::vec2 uv;
    };
    
    class Object
    {
    private:
        std::vector<Triangle> triangles;
        const std::vector<Vertex> vertices;
        const std::vector<uint> indices;
    public:
        Object(const std::vector<Vertex>& vertices, const std::vector<uint>& indices);
        ~Object();
        
        
        /// @brief Locks the frame and converts vertex data to triangles
        void GenerateTriangles();

        /// @brief 
        /// @param ray 
        /// @return 
        bool Intersect(const Ray& ray, Surface& hit);

        Frame frame;
    };

    Object LoadModel(const std::string& filepath);
    
} // namespace Tint
