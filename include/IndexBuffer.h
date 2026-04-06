#pragma once

class IndexBuffer{
private:
    unsigned int m_RendererID; // the ID of the renderer (if you were to use different graphics APIs)
    unsigned int m_Count; // m_ means member I think
public:
    IndexBuffer(const unsigned int* data, unsigned int count); // it makes a lot of sense to use "size" for the amount of bytes and
    // "count" for how many things there are
    // you could also make more constructors to use chars and shorts, but no need here
    ~IndexBuffer();

    // You would have other functions in an actual engine like locking methods and such
    void Bind() const;
    void Unbind() const;

    inline unsigned int GetCount() const {return m_Count; }
};