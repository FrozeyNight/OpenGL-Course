#pragma once

class VertexBuffer{
private:
    unsigned int m_RendererID; // the ID of the renderer (if you were to use different graphics APIs)
public:
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();

    // You would have other functions in an actual engine like locking methods and such
    void Bind() const;
    void Unbind() const;
};