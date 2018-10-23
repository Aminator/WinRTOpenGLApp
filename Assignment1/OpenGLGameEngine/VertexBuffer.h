#pragma once

#include "pch.h"
#include "GLObject.h"

namespace OpenGLGameEngine
{
    template<typename T>
    class VertexBuffer : public GLObject
    {
    private:
        int m_count{};

    public:
        VertexBuffer(const T* data, int count)
            : m_count(count)
        {
            glGenBuffers(1, &m_rendererId);

            Bind();
            glBufferData(GL_ARRAY_BUFFER, count * sizeof(T), data, GL_STATIC_DRAW);
        }

        ~VertexBuffer() override
        {
            glDeleteBuffers(1, &m_rendererId);
        }

        int Count() const { return m_count; }

        void Bind() const override
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_rendererId);
        }

        void Unbind() const override
        {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    };
}
