#include "mesh.hpp"

#include <OBJ_Loader.h>

Tint::Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<uint> &indices)
    : vertices(vertices), indices(indices)
{
}

Tint::Mesh::~Mesh()
{
}

void Tint::Mesh::AddTriangle(const Vertex &v1, const Vertex &v2, const Vertex &v3)
{
    uint lastIndex = vertices.size();
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    indices.push_back(lastIndex - 1);
    indices.push_back(lastIndex);
    indices.push_back(lastIndex + 1);
}

void Tint::Mesh::AddVertices(const std::vector<Vertex> &v)
{
    vertices.insert(vertices.end(), v.begin(), v.end());
}

void Tint::Mesh::AddIndices(const std::vector<uint> &i)
{
    indices.insert(indices.end(), i.begin(), i.end());
}

std::vector<Tint::Mesh> Tint::Mesh::ReadFrom(const std::string &filepath)
{
    std::vector<Mesh> meshes;
    objl::Loader loader;
    
    if (loader.LoadFile(filepath))
    {
        for (auto &&mesh : loader.LoadedMeshes)
        {
            std::vector<Vertex> vertices;
            for (auto &&vert : mesh.Vertices)
            {
                vertices.push_back(Vertex{
                    glm::vec3(vert.Position.X, vert.Position.Y, vert.Position.Z),
                    glm::vec3(vert.Normal.X, vert.Normal.Y, vert.Normal.Z)
                });
            }
    
            //TODO: Incorporate material loading

            meshes.push_back(Mesh(vertices, mesh.Indices));
        }        
    }
    else
    {
        TRaiseError("Could not load model '" +  filepath + "'", "Mesh::ReadFrom");
    }
    
    return meshes;
}

std::vector<Tint::Vertex> Tint::Mesh::GetVertices() const
{
    return vertices;
}

std::vector<uint> Tint::Mesh::GetIndices() const
{
    return indices;
}
