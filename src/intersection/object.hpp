#include "../utils/frame.hpp"
#include "triangle.hpp"
#include "aabb.hpp"
#include <vector>
#include <string>

namespace Tint
{
    struct Ray;
    
    class Object
    {
    private:
        std::vector<Triangle> triangles;
        AABB bounds;
        const std::vector<Vertex> vertices;
        const std::vector<uint> indices;
    public:
    
        std::string name;

        Object(const std::vector<Vertex>& vertices, const std::vector<uint>& indices);
        ~Object();
        
        
        /// @brief Locks the frame and converts vertex data to triangles
        void GenerateTriangles();

        std::vector<Triangle> GetGeneratedTriangles() const;

        AABB GetBounds();

        /// @brief 
        /// @param ray 
        /// @return 
        bool Intersect(const Ray& ray, Surface& hit);

        Frame frame;
    };

    std::vector<Object> LoadModel(const std::string& filepath);
    
} // namespace Tint
