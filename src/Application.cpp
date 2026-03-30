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

    // openGL is a state machine, which means that you set states and enable or disable them (for the most part) 

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

    // enables vertex attributes
    glEnableVertexAttribArray(0);
    // in openGL order doesn't matter as long as you bind the buffer you're working on

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)0);
    // index is where it should be in the "array" of the shader, since it calls them more or less one by one
    // size is basically in how many dimensions you want to draw something (i think) (how many attributes in one vertex?)
    // type - in what variable type is the data
    // do you want to normalize the data to a float (I think)
    // stride is how many bytes does it have to jump to get to another vertex
    // pointer is how many bytes does it have to jump to get to this attribute in each vertex
    // the pointer has to be well a const pointer, so you have to cast it (0 specifically doesn't but figured I should show it here)

    // the vertex shader is called first and determines where exactly to put each vertex on the screen and prepares attributes for the pixel shader (primarily, it does other stuff too)
    // the fragment (or pixel) shader rastersizes the image (actually draws and chooses the color of every pixel on screen). Gets called for each pixel on screen
    // because the fragment shader may be called millions of times you should optimize it and do most heavy calculations in the vertex shader
    // since you can pass data from the vertex shader to the pixel shader

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