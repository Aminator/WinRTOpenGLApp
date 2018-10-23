#pragma once

#include "pch.h"

namespace OpenGLGameEngine
{
    class VertexBufferElement
    {
    private:
        int m_count{};
        bool m_normalized{};

    public:
        VertexBufferElement(int count, bool normalized)
            : m_count(count), m_normalized(normalized)
        {
        }

        int Count() const { return m_count; }
        bool Normalized() const { return m_normalized; }
    };
}
