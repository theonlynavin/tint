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

int main_old(int argc, char const *argv[])
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

    Tint::Image img(WIDTH, HEIGHT);

    Tint::Camera cam(glm::vec2(WIDTH, HEIGHT), M_PI_2, 2, 0);
    cam.LookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0));
    cam.frame.LockTransform();

    Tint::gl_Camera glcam = cam.ToGLCamera();

    Tint::Scene scene;
    std::vector<Tint::Object> model = Tint::LoadModel("assets/dragon.obj");
    // scene.AddObjects(model);
    //model = Tint::LoadModel("assets/scene.obj");
    // model[0].frame.Scale(glm::vec3(2));
    // model[0].frame.Translate(glm::vec3(1, -1, 2));
    scene.AddObjects(model);

    auto nodes = scene.BuildLBVH();

    Tint::Buffer bvh_bounds_min;
    Tint::Buffer bvh_bounds_max;
    Tint::Buffer bvh_data;

    std::vector<glm::vec4> v_bvh_bounds_min; v_bvh_bounds_min.reserve(nodes.first.size());
    std::vector<glm::vec4> v_bvh_bounds_max; v_bvh_bounds_max.reserve(nodes.first.size());
    std::vector<glm::ivec2> v_bvh_data; v_bvh_data.reserve(nodes.first.size());

    for (size_t i = 0; i < nodes.first.size(); i++)
    {
        Tint::gl_BVHNode node = nodes.first[i];
        v_bvh_bounds_min.emplace_back(glm::vec4(node.aabb_min, node.is_leaf));
        v_bvh_bounds_max.emplace_back(glm::vec4(node.aabb_max, 0));

        if (node.is_leaf)
        {
            v_bvh_data.emplace_back(glm::ivec2(node.tri_offset, node.tri_count));
        }
        else
        {
            v_bvh_data.emplace_back(glm::ivec2(node.first_child, node.second_child));
        }
    }

    std::cout << v_bvh_bounds_min[0].x << ", " << v_bvh_bounds_min[0].y << ", " << v_bvh_bounds_min[0].z << ", " << v_bvh_bounds_min[0].w << ", " << std::endl;

    bvh_bounds_min.Allocate(v_bvh_bounds_min.size() * sizeof(glm::vec4), Tint::Buffer::BufferType::SSBO);
    bvh_bounds_max.Allocate(v_bvh_bounds_max.size() * sizeof(glm::vec4), Tint::Buffer::BufferType::SSBO);
    bvh_data.Allocate(v_bvh_data.size() * sizeof(glm::ivec2), Tint::Buffer::BufferType::SSBO);

    bvh_bounds_min.Store(v_bvh_bounds_min.data(), v_bvh_bounds_min.size() * sizeof(glm::vec4), 0);
    bvh_bounds_max.Store(v_bvh_bounds_max.data(), v_bvh_bounds_max.size() * sizeof(glm::vec4), 0);
    bvh_data.Store(v_bvh_data.data(), v_bvh_data.size() * sizeof(glm::ivec2), 0);

    Tint::Buffer tri_v0;
    Tint::Buffer tri_v1;
    Tint::Buffer tri_v2;
    Tint::Buffer tri_n1;
    Tint::Buffer tri_n2;

    std::vector<glm::vec4> v_tri_v0; v_tri_v0.reserve(nodes.second.size());
    std::vector<glm::vec4> v_tri_v1; v_tri_v1.reserve(nodes.second.size());
    std::vector<glm::vec4> v_tri_v2; v_tri_v2.reserve(nodes.second.size());
    std::vector<glm::vec4> v_tri_n1; v_tri_n1.reserve(nodes.second.size());
    std::vector<glm::vec4> v_tri_n2; v_tri_n2.reserve(nodes.second.size());

    for (size_t i = 0; i < nodes.second.size(); i++)
    {
        Tint::Triangle tri = nodes.second[i];
        v_tri_v0.emplace_back(glm::vec4(tri.v1.position, tri.v1.normal.x));
        v_tri_v1.emplace_back(glm::vec4(tri.v2.position, tri.v1.normal.y));
        v_tri_v2.emplace_back(glm::vec4(tri.v3.position, tri.v1.normal.z));
        v_tri_n1.emplace_back(glm::vec4(tri.v2.normal, tri.v3.normal.x));
        v_tri_n2.emplace_back(glm::vec4(0, tri.v3.normal.yz(), 0));
    }
    
    tri_v0.Allocate(nodes.second.size() * sizeof(glm::vec4), Tint::Buffer::BufferType::SSBO);
    tri_v1.Allocate(nodes.second.size() * sizeof(glm::vec4), Tint::Buffer::BufferType::SSBO);
    tri_v2.Allocate(nodes.second.size() * sizeof(glm::vec4), Tint::Buffer::BufferType::SSBO);
    tri_n1.Allocate(nodes.second.size() * sizeof(glm::vec4), Tint::Buffer::BufferType::SSBO);
    tri_n2.Allocate(nodes.second.size() * sizeof(glm::vec4), Tint::Buffer::BufferType::SSBO);

    tri_v0.Store(v_tri_v0.data(), v_tri_v0.size() * sizeof(glm::vec4), 0);
    tri_v1.Store(v_tri_v1.data(), v_tri_v1.size() * sizeof(glm::vec4), 0);
    tri_v2.Store(v_tri_v2.data(), v_tri_v2.size() * sizeof(glm::vec4), 0);
    tri_n1.Store(v_tri_n1.data(), v_tri_n1.size() * sizeof(glm::vec4), 0);
    tri_n2.Store(v_tri_n2.data(), v_tri_n2.size() * sizeof(glm::vec4), 0);

    end = std::chrono::high_resolution_clock::now();
    
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
    shader.BindSSBO("BVHBoundsMin", bvh_bounds_min, 1);
    shader.BindSSBO("BVHBoundsMax", bvh_bounds_min, 2);
    shader.BindSSBO("BVHChildren", bvh_data, 3);
    shader.BindSSBO("TriangleV0", tri_v0, 4);
    shader.BindSSBO("TriangleV1", tri_v1, 5);
    shader.BindSSBO("TriangleV2", tri_v2, 6);
    shader.BindSSBO("TriangleN1", tri_n1, 7);
    shader.BindSSBO("TriangleN2", tri_n2, 8);

    Tint::Shader screenQuad(
        {std::make_pair("assets/shaders/screen_vert.glsl", Tint::Shader::ShaderType::VERTEX),
         std::make_pair("assets/shaders/screen_frag.glsl", Tint::Shader::ShaderType::FRAGMENT)});
    screenQuad.Compile();
    screenQuad.Use();
    screenQuad.SetInt("tex", 0);

    Tint::Texture computeOutput;
    computeOutput.Create(WIDTH, HEIGHT, Tint::Image::Format::RGBA32F);
    computeOutput.Bind(0);

    shader.SetImage("imgOutput", computeOutput);

    end = std::chrono::high_resolution_clock::now();
    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms to initialize shaders!" << std::endl;

    //GL_CALL(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

    bool rendered = false;
    while (!glfwWindowShouldClose(window))
    {
        auto startRender = glfwGetTime();
        shader.Use();
        shader.SetInt("frame_count", (int)glfwGetTime());
        GL_CALL(glDispatchCompute((unsigned int)WIDTH/8+1, (unsigned int)HEIGHT/8+1, 1));

        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        screenQuad.Use();
        renderQuad();

        glfwSwapBuffers(window);
        glfwPollEvents();
        auto finishRender = glfwGetTime();

        std::cout << "Took " << finishRender - startRender << "s to render!\n";
        
        rendered = true;

    }
    
    return 0;
}

int main(int argc, char const *argv[])
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
    cam.LookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0));
    cam.frame.LockTransform();

    Tint::gl_Camera glcam = cam.ToGLCamera();

    Tint::Scene scene;
    std::vector<Tint::Object> model = Tint::LoadModel("assets/dragon.obj");
    // scene.AddObjects(model);
    //model = Tint::LoadModel("assets/scene.obj");
    model[0].frame.Scale(glm::vec3(2));
    // model[0].frame.Translate(glm::vec3(1, -1, 2));
    scene.AddObjects(model);

    auto nodes = scene.BuildLBVH();


    struct gl_BVHNode
    {
        glm::vec4 bounds_min;  // .w contains node type (0=interior, 1=leaf)
        glm::vec4 bounds_max;  // .w unused
        glm::ivec2 children_or_tris; // For interior: (child_first, child_second)
                                // For leaf: (tri_offset, tri_count)
    };

    struct gl_Triangle
    {
        glm::vec4 v0;  // xyz=pos0, w=normal0.x
        glm::vec4 v1;  // xyz=pos1, w=normal0.y
        glm::vec4 v2;  // xyz=pos2, w=normal0.z
        glm::vec4 n0;  // xyz=normal1, w=normal2.x
        glm::vec4 n1;  // xyz=normal1, w=normal2.x
        glm::vec4 n2;  // yz=normal2.yz, w=material_id
    };

    std::vector<gl_BVHNode> v_bvh; v_bvh.reserve(nodes.first.size());

    for (size_t i = 0; i < nodes.first.size(); i++)
    {
        Tint::gl_BVHNode node = nodes.first[i];

        if (node.is_leaf)
        {
            v_bvh.emplace_back(gl_BVHNode{
                glm::vec4(node.aabb_min, 1), 
                glm::vec4(node.aabb_max, 0),
                glm::ivec2(node.tri_offset, node.tri_count)
            });
        }
        else
        {
            v_bvh.emplace_back(gl_BVHNode{
                glm::vec4(node.aabb_min, 0), 
                glm::vec4(node.aabb_max, 0),
                glm::ivec2(node.first_child, node.second_child)
            });
        }
    }

    std::vector<gl_Triangle> v_tri; v_tri.reserve(nodes.second.size());

    for (size_t i = 0; i < nodes.second.size(); i++)
    {
        Tint::Triangle tri = nodes.second[i];
        v_tri.emplace_back(
            gl_Triangle{
                glm::vec4(tri.v1.position, 0),
                glm::vec4(tri.v2.position, 0),
                glm::vec4(tri.v3.position, 0),
                glm::vec4(tri.v1.normal, 0),
                glm::vec4(tri.v2.normal, 0),
                glm::vec4(tri.v3.normal, 0)
            }
        );
    }
    
    Tint::Buffer bvh_buffer;
    Tint::Buffer tri_buffer;

    bvh_buffer.Allocate(v_bvh.size() * sizeof(gl_BVHNode), Tint::Buffer::BufferType::SSBO);
    bvh_buffer.Store(v_bvh.data(), v_bvh.size() * sizeof(gl_BVHNode), 0);

    tri_buffer.Allocate(v_tri.size() * sizeof(gl_Triangle), Tint::Buffer::BufferType::SSBO);
    tri_buffer.Store(v_tri.data(), v_tri.size() * sizeof(gl_Triangle), 0);

    end = std::chrono::high_resolution_clock::now();
    
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
    shader.SetInt("total_tris", v_tri.size());
    shader.BindSSBO("BVHNodes", bvh_buffer, 1);
    shader.BindSSBO("TriangleData", tri_buffer, 2);

    Tint::Shader screenQuad(
        {std::make_pair("assets/shaders/screen_vert.glsl", Tint::Shader::ShaderType::VERTEX),
         std::make_pair("assets/shaders/screen_frag.glsl", Tint::Shader::ShaderType::FRAGMENT)});
    screenQuad.Compile();
    screenQuad.Use();
    screenQuad.SetInt("tex", 0);

    Tint::Texture computeOutput;
    computeOutput.Create(WIDTH, HEIGHT, Tint::Image::Format::RGBA32F);
    computeOutput.Bind(0);

    shader.SetImage("imgOutput", computeOutput);

    end = std::chrono::high_resolution_clock::now();
    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms to initialize shaders!" << std::endl;

    //GL_CALL(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

    bool rendered = false;
    while (!glfwWindowShouldClose(window))
    {
        auto startRender = glfwGetTime();
        shader.Use();
        shader.SetInt("frame_count", (int)glfwGetTime());
        GL_CALL(glDispatchCompute((unsigned int)WIDTH/8+1, (unsigned int)HEIGHT/8+1, 1));

        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        GL_CALL(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
        
        screenQuad.Use();
        renderQuad();

        glfwSwapBuffers(window);
        glfwPollEvents();
        auto finishRender = glfwGetTime();

        std::cout << "Took " << finishRender - startRender << "s to render!\n";
        
        rendered = true;

    }
    
    return 0;
}
