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

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"

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

        /* Blending
        what should OpenGL do when you're trying to apply color with a certain transparency on top of some other color?
        Blending is disabled by default, so it will just overwrite the previous color with the new one
        You first enable blending using glEnable(GL_BLEND)
        Then you can set the blending function glBlendFunc(src, dest):

        src is what values are we taking from the source color (the one we want to render on top of the other color)
        here we're doing GL_SRC_ALPHA, which means we take all the information about color and transparency

        dest is what we want to do with the destination color (the one we're rendering on top of)
        here we use GL_ONE_MINUS_SRC_ALPHA, 
        which means we take the color value of each color of src and multiply it by the src transparency (ALPHA) value,
        then we add to it the dest color value multiplied by ONE MINUS the SRC ALPHA

        so we mathematically get the color that would result from combining these 2 colors,
        but you can use different blending options to obtain different results

        people in the comments pointed out that the GL_ONE_MINUS_SRC_ALPHA works slightly different for the alpha channel
        (since the one showed in the video would produce a 0.75 transparency color from a 1.0 alpha and a 0.5 alpha,
        which is apparently wrong)
        the correct one should apparently be out_alpha = src_alpha + dest_alpha * (1 - src_alpha)
        */

        /* Maths
        In graphics programming we mainly use 2 kinds of math
        Matrices and Vectors
        Vectors not necessarily the "arrow" type that's taught in school
        vector can refer to just a point in space (like vector<200,100> meaning a pixel 200 from the left, 100 up)

        If you're making your own engine, you probably want to write your own library,
        but here we'll use the glm library for simplicity
        */

        GLCall(glEnable(GL_BLEND));
        // Blending (making it so transparency works on textures)
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

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

        // Projection Matrix
        
        // creating a projection matrix to make the image not stretch to the 4:3 aspect ratio of the window
        // (because we set the window size to 400px by 300px)
        glm::mat4 projectionMatrix = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
        // the first 4 numbers will give a 4:3 aspect ratio (if you multiply them by 2 you get 4,4,3,3)
        // the reason they're halved is to make the image larger
        // the positions refer to the edges of the window from left to top
        // the last 2 numbers go into play if we try to put something outside of the edges (will be explained later)

        // ortho - orthographic matrix which we use to parse (map all the coords of) a 3D world onto a 2D screen
        // specifically in a way that doesn't make objects that are further away smaller
        // in 3D rendering you would use a perspective matrix to make objects smaller the further away they are

        // we get the result by multiplying the positions we have by this matrix

        // we do this to get a normalized space (coordinate system, one that goes from -1 to 1 in every axis)
        // it allows us to specify a coordinate system that's different from -1 to 1,
        // for example 1920x1080 and then project it to a -1 to 1 scale, so it can be rendered correctly
        // if you make something outside the -1 to 1 scale, it will not get rendered

        // it's also very important for perspective matrixes, since it can project depth (Z axis)

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
        shader.SetUniformMat4f("u_MVP", projectionMatrix);

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