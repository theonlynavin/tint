#include "tint.hpp"
#include <GLFW/glfw3.h>
#include <chrono>

#define WIDTH 1200
#define HEIGHT 900
#define spp 1

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            -1.0f,
            -1.0f,
            0.0f,
            0.0f,
            0.0f,
            1.0f,
            1.0f,
            0.0f,
            1.0f,
            1.0f,
            1.0f,
            -1.0f,
            0.0f,
            1.0f,
            0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

int main_cpu(int argc, char const *argv[])
{
    auto begin = std::chrono::high_resolution_clock::now();

    Tint::Image img(WIDTH, HEIGHT);

    Tint::Camera cam(glm::vec2(WIDTH, HEIGHT), M_PI_2, 2, 0);
    cam.LookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0));
    cam.frame.LockTransform();

    Tint::Scene scene;
    std::vector<Tint::Object> model = Tint::LoadModel("assets/tyra.obj");
    model[0].frame.Scale(glm::vec3(0.5));
    model[0].frame.Rotate(glm::vec3(0, M_PI, 0));
    scene.AddObjects(model);
    model = Tint::LoadModel("assets/cube.obj");
    model[0].frame.Scale(glm::vec3(0.4));
    model[0].frame.Translate(glm::vec3(1.5, 0, 0));
    scene.AddObjects(model);
    scene.BuildBVH();

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms to initialize the scene!\n";

    begin = std::chrono::high_resolution_clock::now();
    Tint::RandomState state(0);
    for (size_t i = 0; i < WIDTH; i++)
    {
        for (size_t j = 0; j < HEIGHT; j++)
        {
            float u = float(i) / WIDTH;
            float v = 1 - float(j) / HEIGHT;
            Tint::Surface s;
            Tint::Ray r = cam.GenerateRay(u, v, state);
            if (scene.ClosestIntersection(r, s))
                img.SetPixel(i, j, glm::vec4(glm::vec3(glm::length(s.hit.point(s.uv) - r.origin)/10), 1));
        }        
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms to render!\n";
    
    img.SaveAs("./out/test.png");

    return 0;
}

int main_opengl(int argc, char const *argv[])
{
    auto begin = std::chrono::high_resolution_clock::now();
    if (glfwInit() != GLFW_TRUE)
    {
        Tint::TRaiseError("Could not initialize GLFW Window!", "Tint");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Tint", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    Tint::GLInitialize();
    
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms to initialize OpenGL!\n";

    begin = std::chrono::high_resolution_clock::now();

    Tint::Camera cam(glm::vec2(WIDTH, HEIGHT), M_PI_2, 2, 0);
    cam.LookAt(glm::vec3(2, 2, 5), glm::vec3(0, 0, 0));
    cam.frame.LockTransform();

    Tint::gl_Camera glcam = cam.ToGLCamera();

    Tint::Scene scene;
    std::vector<Tint::Object> model = Tint::LoadModel("assets/dragon.obj");
    model[0].frame.Scale(glm::vec3(0.5));
    scene.AddObjects(model);
    model = Tint::LoadModel("assets/bunny.obj");
    model[0].frame.Scale(glm::vec3(0.4));
    model[0].frame.Translate(glm::vec3(-0.5, 0, 2));
    scene.AddObjects(model);

    end = std::chrono::high_resolution_clock::now();
    
    auto lbvh = scene.BuildLBVH();
    auto&& v_bvh = lbvh.first;
    auto&& v_tri = lbvh.second;
    
    Tint::Buffer bvh_buffer;
    Tint::Buffer tri_buffer;
    Tint::Texture bvh_texture(Tint::Texture::Kind::Buffer, Tint::Image::Format::RGBA32F); 
    Tint::Texture tri_texture(Tint::Texture::Kind::Buffer, Tint::Image::Format::RGBA32F);

    bvh_buffer.Allocate(v_bvh.size() * sizeof(Tint::gl_BVHNode), Tint::Buffer::BufferType::TBO);
    bvh_buffer.Store(v_bvh.data(), v_bvh.size() * sizeof(Tint::gl_BVHNode), 0);
    bvh_buffer.Attach(bvh_texture);

    tri_buffer.Allocate(v_tri.size() * sizeof(Tint::gl_Triangle), Tint::Buffer::BufferType::TBO);
    tri_buffer.Store(v_tri.data(), v_tri.size() * sizeof(Tint::gl_Triangle), 0);
    tri_buffer.Attach(tri_texture);

    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms to setup the scene!\n";

    begin = std::chrono::high_resolution_clock::now();

    Tint::Shader shader({std::make_pair("assets/shaders/main.glsl", Tint::Shader::ShaderType::COMPUTE)});
    shader.Compile();
    shader.Use();
    shader.SetFloat("camera.position", glcam.position);
    shader.SetFloat("camera.fwd", glcam.fwd);
    shader.SetFloat("camera.up", glcam.up);
    shader.SetFloat("camera.right", glcam.right);
    shader.SetFloat("camera.scale", glcam.scale);
    shader.SetFloat("camera.aspect_ratio", glcam.aspect);
    shader.SetFloat("camera.aperture", glcam.aperture);
    shader.SetFloat("camera.focal_length", glcam.focal_length);
    shader.SetTexture("bvhNodesTex", bvh_texture);
    shader.SetTexture("trianglesTex", tri_texture);

    Tint::Shader screenQuad(
        {std::make_pair("assets/shaders/screen_vert.glsl", Tint::Shader::ShaderType::VERTEX),
         std::make_pair("assets/shaders/screen_frag.glsl", Tint::Shader::ShaderType::FRAGMENT)});
    screenQuad.Compile();
    screenQuad.Use();
    screenQuad.SetInt("tex", 0);

    Tint::Texture computeOutput(Tint::Texture::Kind::Image2D, Tint::Image::Format::RGBA32F);
    computeOutput.Create(WIDTH, HEIGHT, Tint::Image::Format::RGBA32F);

    Tint::Texture computeAccum(Tint::Texture::Kind::Image2D, Tint::Image::Format::RGBA32F);
    computeAccum.Create(WIDTH, HEIGHT, Tint::Image::Format::RGBA32F);

    shader.BindImage("imgOutput", computeOutput);
    shader.BindImage("accumOutput", computeAccum);

    end = std::chrono::high_resolution_clock::now();
    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms to initialize shaders!" << std::endl;

    int frame = 0;
    while (!glfwWindowShouldClose(window))
    {
        auto startRender = glfwGetTime();
        shader.Use();
        shader.SetInt("frame_count", frame++);
        GL_CALL(glDispatchCompute((unsigned int)WIDTH/16+1, (unsigned int)HEIGHT/16+1, 1));

        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        GL_CALL(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
        
        screenQuad.Use();
        renderQuad();

        glfwSwapBuffers(window);
        glfwPollEvents();
        auto finishRender = glfwGetTime();

        std::cout << "Took " << finishRender - startRender << "s to render!\n";
    }
    
    return 0;
}

int main(int argc, char const *argv[])
{
    return main_opengl(argc, argv);
}