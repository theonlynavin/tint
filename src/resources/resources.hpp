#pragma once
#include "../utils/utils.hpp"
#include "mesh.hpp"
#include "image.hpp"
#include <unordered_map>

namespace Tint
{
    class Image;
    class Mesh;

    typedef std::shared_ptr<Image> ImagePtr;
    typedef std::shared_ptr<Mesh> MeshPtr;

    class Resources
    {
    private:
        static std::unordered_map<std::string, ImagePtr> loadedImages;
        static std::unordered_map<std::string, std::vector<MeshPtr>> loadedModels;
    public:
        static std::vector<MeshPtr> GetModel(const std::string &filepath);
        static ImagePtr GetImage(const std::string &filepath);

        static void AddImage(const Image &image, const std::string &identifier);
        static void AddMesh(const Mesh &mesh, const std::string &identifier);
    };

    ImagePtr LoadImage(const std::string& filepath);
    std::vector<MeshPtr> LoadModel(const std::string& filepath);
} // namespace Tint
