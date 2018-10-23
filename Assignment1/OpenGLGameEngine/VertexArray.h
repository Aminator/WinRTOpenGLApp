#pragma once

#include "pch.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "TypeHelper.h"

namespace OpenGLGameEngine
{
    template<typename T, typename S>
    class VertexArray : public GLObject
    {
    private:
        int m_count{};
        int m_indexCount{};

    public:
        VertexArray(const VertexBufferLayout<T>& layout, const VertexBuffer<T>& vertexBuffer, const IndexBuffer<S>& indexBuffer)
            : m_count(vertexBuffer.Count()), m_indexCount(indexBuffer.Count())
        {
            glGenVertexArrays(1, &m_rendererId);

            Bind();
            vertexBuffer.Bind();
            indexBuffer.Bind();

            auto elements = layout.Elements();

            for (int i = 0, offset = 0; i < elements.size(); i++)
            {
                auto element = elements[i];
                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i, element.Count(), TypeHelper::GetVertexAttribPointerType<T>(), element.Normalized(), layout.Stride(), (void*)offset);
                offset += element.Count() * sizeof(T);
            }
        }

        ~VertexArray() override
        {
            glDeleteVertexArrays(1, &m_rendererId);
        }

        int Count() const { return m_count; }

        int IndexCount() const { return m_indexCount; }

        void Bind() const override
        {
            glBindVertexArray(m_rendererId);
        }

        void Unbind() const override
        {
            glBindVertexArray(0);
        }
    };
}
