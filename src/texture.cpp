#include "texture.h"

#include "stb_image.h";

Texture::Texture(const std::string& path) 
    : m_RendererID(0), m_FilePath(path), 
    m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{
    stbi_set_flip_vertically_on_load(1); // .png files store data starting from top left, OpenGL expects it to start at bottom left
    // so we have to flip the image vertically
    
    m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4); // 4 means we want R,G,B and A (color + alpha)

    GLCall(glGenTextures(1, &m_RendererID));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

    // These parameters MUST be specified
    // Parameter + letter means the parameter is of that type: i - integer, f - float and so on
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)); // MIN_FILTER is scaling down, LINEAR is snapping
    // the texture i think (linearly resample everything - no clue what that means)
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)); // if the image is too small for what we're putting,
    // it on, it will grow it to fit
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP)); // s means x coordinate for textures
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP)); // t means y
    // uh i think we're saying we don't want to change the area the picture will occupy (so it doesn't go outside the object
    // we're rendering the texture on?)

    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
    // 0 - don't need to know yet
    // GL_RGBA8 - type of channel OpenGL will use for this texture
    // 0 - how many pixels should the border of the image have
    // GL_RGBA - type of channel OpenGL will receive the texture in
    // GL_UNSIGNED_BYTE - what type the channel given above is
    // m_LocalBuffer - texture data or nullptr if you don't have it yet, but want to allocate memory

    GLCall(glBindTexture(GL_TEXTURE_2D, 0));

    if(m_LocalBuffer){
        stbi_image_free(m_LocalBuffer); // in serious app you might want to retain this data in CPU memory for sampling or other things
    }
};
Texture::~Texture(){
    GLCall(glDeleteTextures(1, &m_RendererID));
};

void Texture::Bind(unsigned int slot) const{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}; // there is a number of slots (depending on the OS),
// because you can bind more than 1 texture at once
void Texture::Unbind() const{
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
};