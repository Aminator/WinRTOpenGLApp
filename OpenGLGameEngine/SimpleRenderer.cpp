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
#ifdef IS_UWP
        std::wstring vertexShaderSource =
            LR"(
attribute vec3 vertexPos;
attribute vec4 vertexColor;

uniform float time;

varying vec4 vColor;

void main()
{
    gl_Position = vec4(vertexPos, 1.0);
    vColor = vec4(sin(time), vertexColor.y, vertexColor.z, 1.0);
    vColor = vertexColor;
})";

        std::wstring fragmentShaderSource =
            LR"(
precision mediump float;

varying vec4 vColor;

void main()
{
    gl_FragColor = vColor;
})";
#else
        std::wstring vertexShaderSource =
            LR"(
#version 440 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec4 vertexColor;

uniform float time;

out vec4 vColor;

void main()
{
    gl_Position = vec4(vertexPos, 1.0);
    vec3 opaqueColor = vec3(1.0, 0.0, 0.0);
    vColor = vec4(opaqueColor, sin(time));
})";

        std::wstring fragmentShaderSource =
            LR"(
#version 440 core
out vec4 color;

in vec4 vColor;

void main()
{
    color = vColor;
})";
#endif

        const float vertices[] =
        {
            -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
        };

        const unsigned int indices[] =
        {
            0, 1, 2
        };

        vertexShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///VertexShader.glsl");
        fragmentShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///FragmentShader.glsl");

        m_program = std::make_unique<ShaderProgram>(vertexShaderSource, fragmentShaderSource);

        m_vertexBuffer = std::make_unique<VertexBuffer<float>>(vertices, sizeof(vertices) / sizeof(float));
        m_indexBuffer = std::make_unique<IndexBuffer<unsigned int>>(indices, sizeof(indices) / sizeof(unsigned int));

        VertexBufferLayout<float> layout;
        layout.Add(3);
        layout.Add(4);

        m_vertexArray = std::make_unique<VertexArray<float, unsigned int>>(layout, *m_vertexBuffer, *m_indexBuffer);

        m_vertexArray->Unbind();
        m_vertexBuffer->Unbind();
        m_indexBuffer->Unbind();
    }

    void SimpleRenderer::Draw(float deltaTime)
    {
        Renderer::Clear();
        Renderer::ClearColor(0.2f, 0.1f, 0.6f, 1.0f);

        if (m_program != nullptr && m_vertexArray != nullptr)
        {
            (*m_program)[L"time"].SetValue(m_time);
            Renderer::DrawIndexed(*m_vertexArray, *m_program, GL_TRIANGLES);
        }
    }

    void SimpleRenderer::Update(float deltaTime)
    {
        m_time += deltaTime;
    }

    void SimpleRenderer::UpdateWindowSize(int width, int height)
    {
        Renderer::SetViewport(width, height);
        m_windowWidth = width;
        m_windowHeight = height;
    }
}
