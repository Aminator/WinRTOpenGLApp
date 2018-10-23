#include "pch.h"
#include "SimpleRenderer.h"
#include "Renderer.h"

namespace OpenGLGameEngine
{
    SimpleRenderer::SimpleRenderer()
    {
    }

    concurrency::task<void> SimpleRenderer::LoadContentAsync()
    {
        const float vertices[] =
        {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
        };

        const unsigned int indices[] =
        {
            0, 1, 2,
            2, 3, 0
        };

        std::wstring vertexShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///VertexShader.glsl");
        std::wstring fragmentShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///FragmentShader.glsl");

        m_texture0 = std::make_unique<Texture>(co_await Texture::LoadAsync(L"ms-appx:///Container.jpg"));
        m_texture0->Unbind();

        m_texture1 = std::make_unique<Texture>(co_await Texture::LoadAsync(L"ms-appx:///AwesomeFace.png"));
        m_texture1->Unbind();

        m_program = std::make_unique<ShaderProgram>(vertexShaderSource, fragmentShaderSource);

        m_vertexBuffer = std::make_unique<VertexBuffer<float>>(vertices, sizeof(vertices) / sizeof(float));
        m_indexBuffer = std::make_unique<IndexBuffer<unsigned int>>(indices, sizeof(indices) / sizeof(unsigned int));

        VertexBufferLayout<float> layout;
        layout.Add(3);
        layout.Add(2);

        m_vertexArray = std::make_unique<VertexArray<float, unsigned int>>(layout, *m_vertexBuffer, *m_indexBuffer);

        m_vertexArray->Unbind();
        m_vertexBuffer->Unbind();
        m_indexBuffer->Unbind();

        (*m_program)[L"texture0"].SetValue(0);
        (*m_program)[L"texture1"].SetValue(1);

        glEnable(GL_DEPTH_TEST);
    }

    void SimpleRenderer::Draw(float deltaTime)
    {
        Renderer::Clear();
        Renderer::ClearColor(0.2f, 0.1f, 0.6f, 1.0f);

        if (m_program != nullptr && m_vertexArray != nullptr)
        {
            if (m_texture0 != nullptr)
            {
                m_texture0->Bind(GL_TEXTURE0);
            }

            if (m_texture1 != nullptr)
            {
                m_texture1->Bind(GL_TEXTURE1);
            }

            glm::mat4 viewMatrix = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

            glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)m_windowWidth / m_windowHeight, 0.1f, 100.0f);

            (*m_program)[L"viewMatrix"].SetValue(viewMatrix);
            (*m_program)[L"projectionMatrix"].SetValue(projectionMatrix);

            glm::vec3 cubePositions[] =
            {
                glm::vec3(0.0f,  0.0f,  0.0f),
                glm::vec3(2.0f,  5.0f, -15.0f),
                glm::vec3(-1.5f, -2.2f, -2.5f),
                glm::vec3(-3.8f, -2.0f, -12.3f),
                glm::vec3(2.4f, -0.4f, -3.5f),
                glm::vec3(-1.7f,  3.0f, -7.5f),
                glm::vec3(1.3f, -2.0f, -2.5f),
                glm::vec3(1.5f,  2.0f, -2.5f),
                glm::vec3(1.5f,  0.2f, -1.5f),
                glm::vec3(-1.3f,  1.0f, -1.5f)
            };

            for (int i = 0; i < 10; i++)
            {
                glm::mat4 worldMatrix(1.0f);
                worldMatrix = glm::translate(worldMatrix, cubePositions[i]);

                float angle = 20.0f * i;
                worldMatrix = glm::rotate(worldMatrix, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

                (*m_program)[L"worldMatrix"].SetValue(worldMatrix);

                Renderer::Draw(*m_vertexArray, *m_program, GL_TRIANGLES, 36);
            }
        }
    }

    void SimpleRenderer::Update(float deltaTime)
    {
        m_time += deltaTime;

#if IS_UWP
        using namespace winrt::Windows::System;
        using namespace winrt::Windows::UI::Core;

        auto coreWindow = CoreWindow::GetForCurrentThread();

        float cameraSpeed = 2.5f * deltaTime;

        if (coreWindow.GetAsyncKeyState(VirtualKey::W) != CoreVirtualKeyStates::None)
        {
            m_cameraPos += cameraSpeed * m_cameraFront;
        }

        if (coreWindow.GetAsyncKeyState(VirtualKey::S) != CoreVirtualKeyStates::None)
        {
            m_cameraPos -= cameraSpeed * m_cameraFront;
        }

        if (coreWindow.GetAsyncKeyState(VirtualKey::A) != CoreVirtualKeyStates::None)
        {
            m_cameraPos -= glm::normalize(glm::cross(m_cameraFront, m_cameraUp)) * cameraSpeed;
        }

        if (coreWindow.GetAsyncKeyState(VirtualKey::D) != CoreVirtualKeyStates::None)
        {
            m_cameraPos += glm::normalize(glm::cross(m_cameraFront, m_cameraUp)) * cameraSpeed;
        }
#endif
    }

    void SimpleRenderer::UpdateWindowSize(int width, int height)
    {
        Renderer::SetViewport(width, height);
        m_windowWidth = width;
        m_windowHeight = height;
    }
}
