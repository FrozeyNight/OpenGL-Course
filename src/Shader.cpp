#include "Shader.h"

#include "Renderer.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

Shader::Shader(const std::string& filepath) : m_Filepath(filepath), m_RendererID(0){
    ShaderProgramSource source = ParseShader(filepath);
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
};
Shader::~Shader(){
    GLCall(glDeleteProgram(m_RendererID));
};

ShaderProgramSource Shader::ParseShader(const std::string& filepath){
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

unsigned int Shader::CompileShader(unsigned int type, const std::string& source){
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

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader){
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

void Shader::Bind() const{
    GLCall(glUseProgram(m_RendererID));
};
void Shader::Unbind() const{
    GLCall(glUseProgram(0));
};

void Shader::SetUniform1f(const std::string& name, float value){
    GLCall(glUniform1f(GetUniformLocation(name), value));
};

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3){
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
};

unsigned int Shader::GetUniformLocation(const std::string& name){
    
    if(m_UniformLocationCache.find(name) != m_UniformLocationCache.end()){
        return m_UniformLocationCache[name];
    }
    
    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1){ // there will be instances where this will return -1 like if we haven't used the uniform yet (so we shouldn't ASSERT or crash the program here)
        std::cout << "Warning: uniform '" << name << "' does not exist" << std::endl;
    }

    m_UniformLocationCache[name] = location;
    return location;
};