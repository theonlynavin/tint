#include "tint.hpp"

#define WIDTH 800
#define HEIGHT 600

int main(int argc, char const *argv[])
{
    Tint::Image img(WIDTH, HEIGHT);    

    Tint::Camera cam(glm::vec2(WIDTH, HEIGHT), M_PI_2, 2, 0);
    cam.LookAt(glm::vec3(0,0, 6), glm::vec3(0,0,-1));
    cam.frame.LockTransform();

    Tint::Triangle tri{
        Tint::Vertex{glm::vec3(-1,-1,-1), glm::vec3(1, 0, 0)}, 
        Tint::Vertex{glm::vec3(0.5,-1,-1), glm::vec3(0, 1, 0)},
        Tint::Vertex{glm::vec3(-1,1,-1), glm::vec3(0, 0, 1)}, 
    };
    
    Tint::RandomState state(0);
    for (size_t i = 0; i < WIDTH; i++)
    {
        for (size_t j = 0; j < HEIGHT; j++)
        {
            float u = (float(i) / WIDTH);
            float v = 1 - (float(j) / HEIGHT);
            Tint::Ray prim = cam.GenerateRay(u, v, state);

            glm::vec2 uv; float t;
            if (tri.intersect(prim, uv, t))
                img.SetPixel(i, j, tri.normal(uv));
        }
    }

    img.SaveAs("./test.jpg");
    
    
    return 0;
}
