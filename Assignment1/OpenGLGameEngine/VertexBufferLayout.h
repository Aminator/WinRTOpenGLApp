#pragma once

#include "pch.h"
#include "VertexBufferElement.h"

namespace OpenGLGameEngine
{
    template<typename T>
    class VertexBufferLayout
    {
    private:
        std::vector<VertexBufferElement> m_elements{};
        int m_stride{};

    public:
        const std::vector<VertexBufferElement>& Elements() const { return m_elements; }
        int Stride() const { return m_stride; }

        void Add(int count, bool normalized = false)
        {
            VertexBufferElement element(count, normalized);
            m_elements.push_back(element);
            m_stride += count * sizeof(T);
        }
    };
}
