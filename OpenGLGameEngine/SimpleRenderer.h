#pragma once

#include "pch.h"
#include "ShaderProgram.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace OpenGLGameEngine
{
    class SimpleRenderer
    {
    private:
        std::unique_ptr<ShaderProgram> m_program{ nullptr };
        std::unique_ptr<VertexArray<float, unsigned int>> m_vertexArray{ nullptr };
        std::unique_ptr<VertexBuffer<float>> m_vertexBuffer{ nullptr };
        std::unique_ptr<IndexBuffer<unsigned int>> m_indexBuffer{ nullptr };

        int m_windowHeight{};
        int m_windowWidth{};

        float m_time{};

    public:
        SimpleRenderer();

        concurrency::task<void> LoadContentAsync();
        void Draw(float deltaTime);
        void Update(float deltaTime);
        void UpdateWindowSize(int width, int height);
    };
}
