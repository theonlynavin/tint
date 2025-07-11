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
    objects.insert(objects.end(), list.begin(), list.end());
}

std::shared_ptr<Tint::Material> Tint::Scene::FetchMaterial(const std::string &name)
{
    return materaials[name].first;
}

std::shared_ptr<Tint::Texture> Tint::Scene::FetchTexture(const std::string &name)
{
    return textures[name].first;
}

void Tint::Scene::PushMaterial(const Material &material, const std::string& name)
{
    auto it = materaials.find(name);

    if (it != materaials.end())
    {
        TRaiseWarning("Material with name '" + name + "' already exists! It is being overwritten now.", "Scene::PushMaterial");
    }

    materaials[name] = std::make_pair(std::make_shared<Material>(material), prevMaterialID++);
}

void Tint::Scene::PushTexture(const Texture &texture, const std::string& name)
{
    auto it = textures.find(name);

    if (it != textures.end())
    {
        TRaiseWarning("Texture with name '" + name + "' already exists! It is being overwritten now.", "Scene::PushTexture");
    }

    textures[name] = std::make_pair(std::make_shared<Texture>(texture), prevTextureID++);
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
        triangles.insert(triangles.end(), objTris.begin(), objTris.end());
    }

    bvh = new BVH(triangles);
}

std::pair<std::vector<Tint::gl::BVHNode>, std::vector<Tint::gl::Triangle>> Tint::Scene::BuildLBVH()
{
    BuildBVH();

    auto triangles = bvh->GetTriangles();
    std::vector<gl::Triangle> glTriangles;
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
