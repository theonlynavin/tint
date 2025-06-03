#include "tint.hpp"
#include <chrono>

#define WIDTH 800
#define HEIGHT 600

int main(int argc, char const *argv[])
{
    Tint::Image img(WIDTH, HEIGHT);

    Tint::Camera cam(glm::vec2(WIDTH, HEIGHT), M_PI_2, 2, 0);
    cam.LookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0));
    cam.frame.LockTransform();

    auto model = Tint::LoadModel("assets/dragon.obj")[1];
    model.frame.Scale(glm::vec3(0.6));
    model.frame.Rotate(glm::vec3(0, 0, 0));
    model.frame.Translate(glm::vec3(0, -0.5, 0));
    model.GenerateTriangles();

    auto bvh = Tint::BVH(model.GetGeneratedTriangles());

    auto begin = std::chrono::high_resolution_clock::now();

    Tint::RandomState state(0);
    for (size_t i = 0; i < WIDTH; i++)
    {
        for (size_t j = 0; j < HEIGHT; j++)
        {
            float u = (float(i) / WIDTH);
            float v = 1 - (float(j) / HEIGHT);
            Tint::Ray prim = cam.GenerateRay(u, v, state);

            Tint::Surface surf;

            //if (model.Intersect(prim, surf))
            if (bvh.Traverse(prim, surf))
            {
                float t = prim.tMax;
                img.SetPixel(i, j, glm::vec3(2 * glm::exp(-sqrt(t))));
            }
            else
                img.SetPixel(i, j, glm::vec3(0));
        }
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms!\n";

    img.SaveAs("./test.jpg");

    return 0;
}
