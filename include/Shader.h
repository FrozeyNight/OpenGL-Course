#pragma once

#include <string>
#include <unordered_map>
// normally abstracting shaders is very complicated. You would make a custom shader language that would later be compiled
// by your program to the currently used graphics API and so on.
// But for this series we'll just do a very simple implementation

struct ShaderProgramSource{
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader{
private:
    // for debugging
    std::string m_Filepath;
    unsigned int m_RendererID;
    std::unordered_map<std::string, int> m_UniformLocationCache;
public:
    Shader(const std::string& filepath);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    // Set uniforms
    // This would be done in a much more complicated way to handle all the possible types you might pass
    // but here we just add the ones we need
    void SetUniform1f(const std::string& name, float value);
    void SetUniform4f(const std::string& name, float f0, float f1, float f2, float f3);

private:
    ShaderProgramSource ParseShader(const std::string& filepath);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
    unsigned int GetUniformLocation(const std::string& name);
};