#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

static unsigned int CompileShader(unsigned int type, const std::string& source){
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str(); // this is a pointer, so if source is freed from memory, src will point to random data
    glShaderSource(id, 1, &src, nullptr);
    // id is the "index" of the shader
    // count is how many shader sources are we passing
    // string are the array(s) that contain the source of the shader (needs to be a pointer to a pointer)
    // length is an array of lengths for each source? so here nullptr, because it's just one

    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    // the i in iv means that it wants and integer, the v means that it wants a vector (here just a pointer)

    if(result == GL_FALSE){
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        // you could just do char* message = new char[length]. This is just a C function that you can use instead

        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") <<" shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
};

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader){
    // normally you would probably load the shaders from a file, but here we'll write them out for demonstration purposes
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    // you can use GLuint to use openGL's types, but if you're working with multiple graphics APIs, you'll have to include openGL everywhere
    // so better to just use the standard types

    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    // You can delete shaders now, since they're linked
    // You should technically detach them first, but it's kind of pointless and makes debugging way harder
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

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

    std::string vertexShader = 
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec4 position;\n" // 0 is the index of the attribute assigned above (the 6 floats)
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = position;\n"
        "}\n";

    std::string fragmentShader = 
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 color;\n" // 0 is the index of the attribute assigned above (the 6 floats)
        "\n"
        "void main()\n"
        "{\n"
        "   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";

    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    
    // Biding shader
    glUseProgram(shader);

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

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}