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

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_opengl3.h"
#include "vendor/imgui/imgui_impl_glfw.h"

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
    window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
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
            -50.0f, -50.0f, 0.0f, 0.0f,// 0
             50.0f, -50.0f, 1.0f, 0.0f,// 1
             50.0f,  50.0f, 1.0f, 1.0f,// 2
            -50.0f,  50.0f, 0.0f, 1.0f,// 3
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

        /* Projection Matrix
        
        // creating a projection matrix to make the image not stretch to the 4:3 aspect ratio of the window
        // (because we set the window size to 400px by 300px)
        //glm::mat4 projectionMatrix = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
        // the first 4 numbers will give a 4:3 aspect ratio (if you multiply them by 2 you get 4,4,3,3)
        // the reason they're halved is to make the image larger
        // the positions refer to the edges of the window from left to top
        // the last 2 numbers go into play if we try to put something outside of the edges (will be explained later)
        // leftX, rightX, bottomY, topY, farZ, closeZ basically
        // this also means that 0.0f by 0.0f is the middle (2/4 of width, 1.5/3 of height)

        // ortho - orthographic matrix which we use to parse (map all the coords of) a 3D world onto a 2D screen
        // specifically in a way that doesn't make objects that are further away smaller
        // in 3D rendering you would use a perspective matrix to make objects smaller the further away they are

        // we get the result by multiplying the positions we have by this matrix

        // we do this to get a normalized space (coordinate system, one that goes from -1 to 1 in every axis)
        // it allows us to specify a coordinate system that's different from -1 to 1,
        // for example 1920x1080 and then project it to a -1 to 1 scale, so it can be rendered correctly
        // if you make something outside the -1 to 1 scale (outside the coords of this matrix), it will not get rendered

        // it's also very important for perspective matrixes, since it can project depth (Z axis)
        */

        glm::mat4 projectionMatrix = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);

        /* MVP - Model View Projection Matrices
            These are the 3 matrices that we multiply our vertex positions with everytime we draw something on screen
            M - Model matrix - it's the transform of whatever is being drawn (position, rotation and scale)
            V - View matrix - it's the "camera" (you can adjust the size, position, orientation)
            P - Projection matrix - already explained in detail (projects the 3D world into a 2D scale)

            you multiply each matrix with the vertex positions in a specific order
            in OpenGL it's P*V*M*vertexPosition, so reversed order 

            there is no such thing as a camera in OpenGL, so if you want to simulate the "camera" moving to the left,
            you need to move every oject on screen to the right

            this is basically all the matrix multiplication you need to know to position objects in a 2D or 3D world

            alternative explanation by @fendoroid3788
            So, basically:
            Model matrix: defines position, rotation and scale of the vertices of the model in the world.
            View matrix: defines position and orientation of the "camera".
            Projection matrix: Maps what the "camera" sees to NDC, taking care of aspect ratio and perspective.
        */

        glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        //glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 200, 0));
        //glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0));

        //glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
        //shader.SetUniformMat4f("u_MVP", mvp);

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

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 130");

        glm::vec3 translationA(200, 200, 0);
        glm::vec3 translationB(400, 200, 0);

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            {
                glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), translationA);
                glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
                shader.Bind(); // it's slow if you try to bind a shader that's already bound, but here just for show and a bit more robust
                // normally you would have a thing to check if the shader is already bound or not ^
                //shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
                shader.SetUniformMat4f("u_MVP", mvp);
                renderer.Draw(va, ib, shader);
            }
            

            {
                glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), translationB);
                glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
                shader.Bind();
                //shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
                shader.SetUniformMat4f("u_MVP", mvp);
                renderer.Draw(va, ib, shader);
            }
            /*  Drawing multiple objects

                There are 2 main ways of drawing multiple objects:
                1. supplying the shader with a different vertex buffer <- not shown in the course
                 ^ I think you just need to make a second vertex array (so vertex buffer and index buffer) and pass those to the draw call
                2. changing the MVP Matrix (I think that will only allow to have multiple of the same object?)
                 ^ no because you could still change the texture and stuff in the shader
                  ^ but that would only make it possible to change the texture, not the underlying shape, so I'm not sure
            */

            if(r > 1.0f){
                increment = -0.02f;
            }
            else if(r < 0.05f){
                increment = 0.02f;
            }

            r += increment;

            static float f = 0.0f;
            static int counter = 0;

            // ImGui debug window elements
            {
                ImGui::Begin("Debug Window"); // Create a window called "Hello, world!" and append into it.

                ImGui::SliderFloat3("Translation A", &translationA.x, 0.0f, 960.0f); // Edit 1 float using a slider from 0.0f to 960.0f
                ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f, 960.0f); // Edit 1 float using a slider from 0.0f to 960.0f

                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
                ImGui::End();
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}