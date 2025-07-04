#include "scene.hpp"

Tint::Scene::Scene()
    : bvh(nullptr)
{
}

Tint::Scene::~Scene()
{
    if (bvh)
        delete bvh;
}

void Tint::Scene::AddObject(const Object &object)
{
    objects.push_back(object);
}

void Tint::Scene::AddObjects(const std::vector<Object>& list)
{
    //objects.reserve(objects.size() + list.size());
    objects.insert(objects.end(), list.begin(), list.end());
}

void Tint::Scene::BuildBVH()
{
    if (bvh)
    {
        delete bvh;
        bvh = nullptr;
    }

    std::vector<Triangle> triangles;
    for (auto& obj : objects)
    {
        obj.GenerateTriangles();
        std::vector<Triangle> objTris = obj.GetGeneratedTriangles();
     
        //triangles.reserve(triangles.size() + objTris.size());
        triangles.insert(triangles.end(), objTris.begin(), objTris.end());
    }

    bvh = new BVH(triangles);
}

std::pair<std::vector<Tint::gl_BVHNode>, std::vector<Tint::gl_Triangle>> Tint::Scene::BuildLBVH()
{
    BuildBVH();

    auto triangles = bvh->GetTriangles();
    std::vector<gl_Triangle> glTriangles;
    glTriangles.reserve(triangles.size());

    for (auto &&tri : triangles)
    {
        glTriangles.emplace_back(tri.ToGLTriangle());
    }

    return std::make_pair(bvh->ToGLBVH(), glTriangles);
}

bool Tint::Scene::ClosestIntersection(Ray& ray, Surface &surface)
{
    return bvh->Traverse(ray, surface);
}
