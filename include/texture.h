#pragma once

#include "Renderer.h"

// usually a serious app using a graphics API would use it's own image format,
// but here we're using .png for simplicity

class Texture{
private:
    unsigned int m_RendererID;
    std::string m_FilePath;
    unsigned char* m_LocalBuffer; // local storage of the texture
    int m_Width, m_Height, m_BPP; // BPP - bits per pixel
public:
    Texture(const std::string& path);
    ~Texture();

    void Bind(unsigned int slot = 0) const; // there is a number of slots (depending on the OS),
    // because you can bind more than 1 texture at once
    void Unbind() const;

    inline int GetWidth() const {return m_Width;}
    inline int GetHeight() const {return m_Height;}
};