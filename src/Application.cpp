#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

// a Render works like this: you give it a command and it renders that thing

struct ShaderProgramSource{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath){
    // using a modern C++ way of reading files, though in something like a game engine you probably want to use the C file API
    std::ifstream stream(filepath);

    enum class ShaderType{
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2]; // we initialize 2 in the same line this way
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)){
        if(line.find("#shader") != std::string::npos){
            if(line.find("vertex") != std::string::npos){
                type = ShaderType::VERTEX;
            }
            else if(line.find("fragment") != std::string::npos){
                type = ShaderType::FRAGMENT;
            }
        }
        else{
            // doing it this way makes it so we don't have to make an if statement that would add the lines to the correct stream
            ss[(int)type] << line << '\n';
        }
    }

    // returning as a ShaderProgramSource struct
    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source){
    GLCall(unsigned int id = glCreateShader(type));
    const char* src = source.c_str(); // this is a pointer, so if source is freed from memory, src will point to random data
    GLCall(glShaderSource(id, 1, &src, nullptr));
    // id is the "index" of the shader
    // count is how many shader sources are we passing
    // string are the array(s) that contain the source of the shader (needs to be a pointer to a pointer)
    // length is an array of lengths for each source? so here nullptr, because it's just one

    GLCall(glCompileShader(id));

    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    // the i in iv means that it wants and integer, the v means that it wants a vector (here just a pointer)
    // this returns a parameter from a shader object

    if(result == GL_FALSE){
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char));
        // you could just do char* message = new char[length]. This is just a C function that you can use instead

        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") <<" shader!" << std::endl;
        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
};

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader){
    // normally you would probably load the shaders from a file, but here we'll write them out for demonstration purposes
    GLCall(unsigned int program = glCreateProgram());
    GLCall(unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader));
    // you can use GLuint to use openGL's types, but if you're working with multiple graphics APIs, you'll have to include openGL everywhere
    // so better to just use the standard types

    GLCall(unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader));

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    // You can delete shaders now, since they're linked
    // You should technically detach them first, but it's kind of pointless and makes debugging way harder
    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

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

    { // creating a scope here so the program doesn't keep running until it's forced to close down because of how glGetError works
      // after the openGL context is deleted
        float positions[] = {
            -0.5f,-0.5f, // 0
            0.5f,-0.5f, // 1
            0.5f, 0.5f, // 2
            -0.5f, 0.5f, // 3
        };

        // using unsigned ints, but if you need to save memory use unsigned char or unsigned short
        // you CAN'T use signed variables for this. It won't work and you won't get any error messages
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        // Creating a Vertex Array
        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

        VertexArray va;
        VertexBuffer vb(positions, 4 * 2 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);


        IndexBuffer ib(indices, 6);

        ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
        /*
        std::cout << "VERTEX" << std::endl;
        std::cout << source.VertexSource << std::endl;
        std::cout << "FRAGMENT" << std::endl;
        std::cout << source.FragmentSource << std::endl;
        */

        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
        
        // Biding shader
        GLCall(glUseProgram(shader));

        // Uniforms are a way for the CPU to give data to the GPU
        // They are set per draw instead of per vertex (which means you can't change uniforms while the draw is happening)
        // To use uniforms a shader must be currently bound (so glUseProgram(shader) must be called before this)

        // This fetches the location of this variable from the shader
        GLCall(int location = glGetUniformLocation(shader, "u_Color"));
        // sometimes even if everything is working as intended this will return -1, because if the uniform is not used
        // the compiler will remove it for optimization 
        ASSERT(location != -1);
        // this sets the fetched variable to something
        GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));
        // the naming convention is glUniform + what you are sending (here it's 4 floats, so 4f)

        float r = 0.0f;
        float increment = 0.02f;

        // Unbinding for demonstration
        GLCall(glBindVertexArray(0));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        GLCall(glUseProgram(0));

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT));

            // clearing errors from before a suspect function is called
            //GLClearError();

            // Drawing using the index buffer
            // This is the main way you display things using OpenGL:
            // Create Vertex Buffer -> Create Index Buffer -> glDrawElements
            //glDrawElements(GL_TRIANGLES, 6, GL_INT, nullptr);
            // count is the number of indices NOT vertices 
            // indices is nullptr here, because it's bound

            // checking for errors as soon as the suspect function is called
            //GLCheckError();
            // This will return an error code in decimal
            // To check what it means go to definition of the opengl/glew header and look for the hexadecimal representation of the error code

            //ASSERT(GLLogCall());

            // You have to bind like this to draw a specific object without using Vertex Arrays
            GLCall(glUseProgram(shader));
            GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

            va.Bind();
            // this is an OpenGL "special", since other APIs don't have this feature

            // These are not needed if you have a vertex array
            //GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
            //GLCall(glEnableVertexAttribArray(0));
            //GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)0));

            //GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
            //ib.Bind();


            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

            if(r > 1.0f){
                increment = -0.02f;
            }
            else if(r < 0.05f){
                increment = 0.02f;
            }

            r += increment;

            // This draws the currently bound buffer (so the one we specified before this loop)
            //glDrawArrays(GL_TRIANGLES, 0, 6);
            // If we would bind a different buffer, it would use that one instead
            // That's why we don't pass the buffer to this function

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

        GLCall(glDeleteProgram(shader));

    }
    glfwTerminate();
    return 0;
}