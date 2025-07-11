#pragma once
#include "../utils/utils.hpp"
#include "../intersection/triangle.hpp"

namespace Tint
{
    class Mesh
    {
    private:
        std::vector<Vertex> vertices;
        std::vector<uint> indices;
    public:
        Mesh(const std::vector<Vertex> &vertices, const std::vector<uint> &indices);
        ~Mesh();

        void AddTriangle(const Vertex &v1, const Vertex &v2, const Vertex &v3);
        void AddVertices(const std::vector<Vertex> &vertices);
        void AddIndices(const std::vector<uint> &indices);

        //TODO: implement void SaveAs(const std::string &filepath);

        static std::vector<Mesh> ReadFrom(const std::string& filepath);

        std::vector<Vertex> GetVertices() const;
        std::vector<uint> GetIndices() const;
    };
} // namespace Tint
