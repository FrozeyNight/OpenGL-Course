#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if(glewInit() != GLEW_OK){
        std::cout << "Error!" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };

    unsigned int buffer;
    
    // Creating 1 buffer and sending it's ID to the "buffer" variable
    glGenBuffers(1, &buffer);
    // everything in openGL has an ID (just an integer, 0 usually means bad)
    // so if you want to make a triangle for example you make 1 buffer for it and then pass it's ID every time you want to draw it

    // selecting in openGL is called binding
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    // "target" means what is the purpose of this (here just an array)

    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);
    // target, size of the buffer in bytes, pointer to the buffer, STATIC(made once but called a lot of times) DRAW(well we want to draw it)
    // the STATIC and DRAW are just hints, so if you for example use STATIC and modify it during runtime it will still work, but will be slower
    // also good documentation docs.GL

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // This draws the currently bound buffer (so the one we specified before this loop)
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // If we would bind a different buffer, it would use that one instead
        // That's why we don't pass the buffer to this function

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}