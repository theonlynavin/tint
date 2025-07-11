#include "resources.hpp"
#include "mesh.hpp"
#include "image.hpp"

std::unordered_map<std::string, std::shared_ptr<Tint::Image>> Tint::Resources::loadedImages;
std::unordered_map<std::string, std::vector<std::shared_ptr<Tint::Mesh>>> Tint::Resources::loadedModels;

std::shared_ptr<Tint::Image> Tint::LoadImage(const std::string &filepath)
{
    auto loaded = Image::ReadFrom(filepath);
    Resources::AddImage(loaded, filepath);

    return Resources::GetImage(filepath);
}

std::vector<std::shared_ptr<Tint::Mesh>> Tint::LoadModel(const std::string &filepath)
{
    auto loaded = Mesh::ReadFrom(filepath);
    for (auto &&m : loaded)
        Resources::AddMesh(m, filepath);

    return Resources::GetModel(filepath);
}


std::vector<std::shared_ptr<Tint::Mesh>> Tint::Resources::GetModel(const std::string &filepath)
{        
    auto it = loadedModels.find(filepath);

    if (it != loadedModels.end())
        return it->second;
    else
    {
        return LoadModel(filepath);
    }
}

std::shared_ptr<Tint::Image> Tint::Resources::GetImage(const std::string &filepath)
{
    auto it = loadedImages.find(filepath);

    if (it != loadedImages.end())
        return it->second;
    else
        return LoadImage(filepath);
}

void Tint::Resources::AddMesh(const Mesh &mesh, const std::string &identifier)
{
    auto it = loadedModels.find(identifier);

    if (it != loadedModels.end())
    {
        loadedModels[identifier].push_back(std::make_shared<Mesh>(mesh));
    }
    else
    {
        loadedModels[identifier] = {std::make_shared<Mesh>(mesh)};
    }
}

void Tint::Resources::AddImage(const Image &image, const std::string &identifier)
{
    loadedImages[identifier] = std::make_shared<Image>(image); 
}
