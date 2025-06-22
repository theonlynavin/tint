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

std::pair<std::vector<Tint::gl_BVHNode>, std::vector<Tint::Triangle>> Tint::Scene::BuildLBVH()
{
    
    /*std::vector<Triangle> mytris;
    for (auto& obj : objects)
    {
        obj.GenerateTriangles();
        std::vector<Triangle> objTris = obj.GetGeneratedTriangles();
     
        //triangles.reserve(triangles.size() + objTris.size());
        mytris.insert(mytris.end(), objTris.begin(), objTris.end());
    }

    LBVH lbvh = LBVH(mytris);*/

    BuildBVH();
    return std::make_pair(bvh->ToGLBVH(), bvh->GetTriangles());
}

bool Tint::Scene::ClosestIntersection(Ray& ray, Surface &surface)
{
    return bvh->Traverse(ray, surface);
}
