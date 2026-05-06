#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "texture.h"

// a Render works like this: you give it a command and it renders that thing

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // Setting the version of OpenGL and choosing the CORE profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // makes the window render in the monitor's? framerate
    glfwSwapInterval(1);

    if(glewInit() != GLEW_OK){
        std::cout << "Error!" << std::endl;
    }

    //std::cout << glGetString(GL_VERSION) << std::endl;

    {
        float positions[] = {
            -0.5f,-0.5f, 0.0f, 0.0f,// 0
            0.5f,-0.5f, 1.0f, 0.0f,// 1
            0.5f, 0.5f, 1.0f, 1.0f,// 2
            -0.5f, 0.5f, 0.0f, 1.0f,// 3
        };
        
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        GLCall(glEnable(GL_BLEND));
        // Blending (making it so transparency works on textures)
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        // GL_SRC_ALPHA - for the source take the source alpha
        // GL_ONE_MINUS_SRC_ALPHA - when rendering something on top of that, take 1 - (source alpha)
        // (so taking the difference of that)

        // Creating a Vertex Array
        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

        VertexArray va;
        VertexBuffer vb(positions, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2); // 2 because we're adding 2 triangles
        layout.Push<float>(2); // for texture coordinates
        va.AddBuffer(vb, layout);


        IndexBuffer ib(indices, 6);

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

        Texture texture("res/textures/Example.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0); // 0 means the slot in which the texture we want to access is

        va.Unbind();
        shader.Unbind();
        vb.Unbind();
        ib.Unbind();

        float r = 0.0f;
        float increment = 0.02f;

        Renderer renderer;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            shader.Bind();
            shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

            renderer.Draw(va, ib, shader);

            if(r > 1.0f){
                increment = -0.02f;
            }
            else if(r < 0.05f){
                increment = 0.02f;
            }

            r += increment;

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

    }
    glfwTerminate();
    return 0;
}