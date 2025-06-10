#include "scene.hpp"
#include "../intersection/lbvh.hpp"

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

std::pair<std::vector<Tint::cl_BVHNode>, std::vector<Tint::cl_Triangle>> Tint::Scene::BuildLBVH()
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
    BVH lbvh = *bvh;

    std::vector<Tint::cl_BVHNode> nodes = lbvh.ToCLBVH();
    std::vector<Tint::cl_Triangle> triangles;
    triangles.reserve(lbvh.GetTriangles().size());
    int numTris = lbvh.GetTriangles().size();
    std::vector<Triangle> tris = lbvh.GetTriangles();

    std::cout << "BVH Flattened!\n";

    std::cout << numTris << " triangles to align!\n"; 

    for (size_t i = 0; i < numTris; i++)
    {
        cl_Triangle cl_tri;
        Triangle& tri = tris[i];
        cl_tri.v0 = cl_float3{tri.v1.position.x, tri.v1.position.y, tri.v1.position.z};
        cl_tri.v1 = cl_float3{tri.v2.position.x, tri.v2.position.y, tri.v2.position.z};
        cl_tri.v2 = cl_float3{tri.v3.position.x, tri.v3.position.y, tri.v3.position.z};
        cl_tri.n0 = cl_float3{tri.v1.normal.x, tri.v1.normal.y, tri.v1.normal.z};
        cl_tri.n1 = cl_float3{tri.v2.normal.x, tri.v2.normal.y, tri.v2.normal.z};
        cl_tri.n2 = cl_float3{tri.v3.normal.x, tri.v3.normal.y, tri.v3.normal.z};
        triangles.emplace_back(cl_tri);
    }

    std::cout << "Triangles aligned!\n";
    
    return std::make_pair(nodes, triangles);
}

bool Tint::Scene::ClosestIntersection(Ray& ray, Surface &surface)
{
    return bvh->Traverse(ray, surface);
}
