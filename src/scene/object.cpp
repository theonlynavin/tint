#include "object.hpp"
#include "../intersection/aabb.hpp"

Tint::Object::Object(const std::vector<MeshMat>& meshes)
    : meshes(meshes)
{
}

Tint::Object::~Object()
{
}

void Tint::Object::GenerateTriangles()
{
    // TODO: a nice way of integrating material IDs to triangle data

    triangles.clear();

    for (auto &&meshmat : meshes)
    {
        auto mesh = meshmat.first;

        std::vector<Vertex> vertices = mesh->GetVertices();
        std::vector<uint> indices = mesh->GetIndices();
    
        frame.LockTransform();
        glm::mat4 transform = frame.GetFrameToWorld();
        glm::mat4 invtransformTranspose = glm::transpose(frame.GetWorldToFrame());
    
        triangles.reserve(triangles.size() + indices.size() / 3);
    
        for (size_t i = 0; i < indices.size(); i += 3)
        {
            Vertex v1 = vertices[indices[i]];
            Vertex v2 = vertices[indices[i+1]];
            Vertex v3 = vertices[indices[i+2]];
    
            v1.position = (transform * glm::vec4(v1.position, 1)).xyz();
            v2.position = (transform * glm::vec4(v2.position, 1)).xyz();
            v3.position = (transform * glm::vec4(v3.position, 1)).xyz();
    
            v1.normal = (invtransformTranspose * glm::vec4(v1.normal, 0)).xyz();
            v2.normal = (invtransformTranspose * glm::vec4(v2.normal, 0)).xyz();
            v3.normal = (invtransformTranspose * glm::vec4(v3.normal, 0)).xyz();
    
            triangles.emplace_back(Triangle{v1,v2,v3});
        }
    
        bounds.Expand(AABB(triangles));
    }
}

std::vector<Tint::Triangle> Tint::Object::GetGeneratedTriangles() const
{
    return triangles;
}

std::vector<Tint::MeshMat> Tint::Object::GetRawMeshes() const
{
    return meshes;
}

void Tint::Object::AddMesh(const std::vector<MeshMat> &m)
{
    meshes.insert(meshes.end(), m.begin(), m.end());
}

Tint::AABB Tint::Object::GetBounds() const
{
    return bounds;
}

bool Tint::Object::Intersect(Ray &ray, Surface &hit) const
{
    bool hitAnything = false;

    float tbox = FLT_MAX;
    if (!bounds.Intersect(ray, tbox))
        return false;

    for (const Triangle& tri : triangles)
    {
        glm::vec2 uv;
        if (tri.Intersect(ray, uv))
        {
            hit.hit = tri;
            hit.uv = uv;
            hitAnything = true;
        }
    }

    return hitAnything;
}
