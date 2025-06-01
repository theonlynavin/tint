#include "object.hpp"

Tint::Object::Object(const std::vector<Vertex> &vertices, const std::vector<uint> &indices)
    : vertices(vertices), indices(indices)
{
}

Tint::Object::~Object()
{
}

void Tint::Object::GenerateTriangles()
{
    frame.LockTransform();
    glm::mat4 transform = frame.GetFrameToWorld();
    glm::mat4 invtransformTranspose = glm::transpose(frame.GetWorldToFrame());

    triangles.clear();
    triangles.reserve(indices.size() / 3);

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        Vertex v1 = vertices[i];
        Vertex v2 = vertices[i+1];
        Vertex v3 = vertices[i+2];

        v1.position = (transform * glm::vec4(v1.position, 1)).xyz();
        v2.position = (transform * glm::vec4(v2.position, 1)).xyz();
        v3.position = (transform * glm::vec4(v3.position, 1)).xyz();

        v1.normal = (invtransformTranspose * glm::vec4(v1.normal, 0)).xyz();
        v2.normal = (invtransformTranspose * glm::vec4(v2.normal, 0)).xyz();
        v3.normal = (invtransformTranspose * glm::vec4(v3.normal, 0)).xyz();

        triangles.emplace_back(Triangle{v1,v2,v3});
    }
}

bool Tint::Object::Intersect(const Ray &ray, Surface &hit)
{
    bool hitAnything = false;
    float minDist = FLT_MAX;

    for (Triangle& tri : triangles)
    {
        float t = 0;
        glm::vec2 uv;
        if (tri.intersect(ray, uv, t))
        {
            if (t < minDist)
            {
                hit.hit = tri;
                hit.uv = uv;
            }

            hitAnything = true;
        }
    }

    return hitAnything;
}
