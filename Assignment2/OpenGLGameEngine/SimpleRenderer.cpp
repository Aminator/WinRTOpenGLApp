#include "pch.h"
#include "SimpleRenderer.h"
#include "Renderer.h"

namespace OpenGLGameEngine
{
    SimpleRenderer::SimpleRenderer()
    {
        glEnable(GL_DEPTH_TEST);

        using namespace winrt::Windows::Foundation;
        using namespace winrt::Windows::System;
        using namespace winrt::Windows::UI::Core;
        using namespace winrt::Windows::Devices::Input;

        CoreWindow coreWindow = CoreWindow::GetForCurrentThread();

        coreWindow.PointerPressed([this](const CoreWindow& sender, const PointerEventArgs& args)
        {
            if (args.CurrentPoint().PointerDevice().PointerDeviceType() == PointerDeviceType::Mouse && !m_isPositionLocked)
            {
                Point lockedPosition = Point(
                    sender.Bounds().X,
                    sender.Bounds().Y);

                lockedPosition.X += sender.Bounds().Width / 2;
                lockedPosition.Y += sender.Bounds().Height / 2;

                sender.PointerPosition(lockedPosition);
                sender.PointerCursor(nullptr);

                m_previousPosition = glm::vec2(lockedPosition.X, lockedPosition.Y);
                m_isPositionLocked = true;
            }
            else
            {
                Point position = args.CurrentPoint().Position();

                position.X += sender.Bounds().X;
                position.Y += sender.Bounds().Y;

                m_previousPosition = glm::vec2(position.X, position.Y);
                m_isPositionLocked = false;
                if (sender.PointerCursor() == nullptr) sender.PointerCursor(CoreCursor(CoreCursorType::Arrow, 0));
            }
        });

        coreWindow.PointerMoved([this](const CoreWindow& sender, const PointerEventArgs& args)
        {
            if (args.CurrentPoint().PointerDevice().PointerDeviceType() == PointerDeviceType::Mouse)
            {
                if (!m_isPositionLocked) return;
            }
            else
            {
                if (!args.CurrentPoint().IsInContact()) return;

                m_isPositionLocked = false;
                if (sender.PointerCursor() == nullptr) sender.PointerCursor(CoreCursor(CoreCursorType::Arrow, 0));
            }

            Point position = args.CurrentPoint().Position();

            position.X += sender.Bounds().X;
            position.Y += sender.Bounds().Y;

            float xOffset = position.X - m_previousPosition.x;
            float yOffset = m_previousPosition.y - position.Y;

            float sensitivity = 0.05f;
            xOffset *= sensitivity;
            yOffset *= sensitivity;

            m_yaw += xOffset;
            m_pitch += yOffset;

            if (m_pitch > 89.0f) m_pitch = 89.0f;
            if (m_pitch < -89.0f) m_pitch = -89.0f;

            glm::vec3 front;
            front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
            front.y = sin(glm::radians(m_pitch));
            front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
            m_cameraFront = glm::normalize(front);

            if (m_isPositionLocked)
            {
                sender.PointerPosition(Point(m_previousPosition.x, m_previousPosition.y));
            }
            else
            {
                m_previousPosition = glm::vec2(position.X, position.Y);
            }
        });

        coreWindow.KeyDown([this](const CoreWindow& sender, const KeyEventArgs& args)
        {
            if (args.VirtualKey() == VirtualKey::Escape)
            {
                m_isPositionLocked = false;
                if (sender.PointerCursor() == nullptr) sender.PointerCursor(CoreCursor(CoreCursorType::Arrow, 0));
            }
        });

        coreWindow.PointerWheelChanged([this](const CoreWindow& sender, const PointerEventArgs& args)
        {
            if (m_fov >= 1.0f && m_fov <= 45.0f) m_fov -= args.CurrentPoint().Properties().MouseWheelDelta() * 0.1f;
            if (m_fov <= 1.0f) m_fov = 1.0f;
            if (m_fov >= 45.0f) m_fov = 45.0f;
        });
    }

    concurrency::task<void> SimpleRenderer::LoadContentAsync()
    {
        const float vertices[] =
        {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
                                               
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
                                               
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
                                               
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
                                               
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
                                               
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f
        };

        const unsigned int indices[] =
        {
            0, 1, 2
        };

        std::wstring vertexShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///Assets/VertexShader.glsl");
        std::wstring fragmentShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///Assets/FragmentShader.glsl");

        std::wstring lampVertexShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///Assets/LightVertexShader.glsl");
        std::wstring lampFragmentShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///Assets/LightFragmentShader.glsl");

        m_texture0 = co_await Texture::LoadAsync(L"ms-appx:///Assets/Container2.png");
        m_texture0->Unbind();

        m_texture1 = co_await Texture::LoadAsync(L"ms-appx:///Assets/Container2_Specular.png");
        m_texture1->Unbind();

        m_program = std::make_unique<ShaderProgram>(vertexShaderSource, fragmentShaderSource);
        m_lightProgram = std::make_unique<ShaderProgram>(lampVertexShaderSource, lampFragmentShaderSource);

        m_vertexBuffer = std::make_unique<VertexBuffer<float>>(vertices, sizeof(vertices) / sizeof(float));
        m_indexBuffer = std::make_unique<IndexBuffer<unsigned int>>(indices, sizeof(indices) / sizeof(unsigned int));

        VertexBufferLayout<float> layout;
        layout.Add(3);
        layout.Add(2);
        layout.Add(3);

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
            if (m_texture0 != nullptr)
            {
                m_texture0->Bind(GL_TEXTURE0);
            }

            if (m_texture1 != nullptr)
            {
                m_texture1->Bind(GL_TEXTURE1);
            }

            glm::mat4 viewMatrix = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

            glm::mat4 projectionMatrix = glm::perspective(glm::radians(m_fov), (float)m_windowWidth / m_windowHeight, 0.1f, 100.0f);

            (*m_program)[L"viewMatrix"].SetValue(viewMatrix);
            (*m_program)[L"projectionMatrix"].SetValue(projectionMatrix);

            (*m_program)[L"viewPosition"].SetValue(m_cameraPos);

            (*m_program)[L"material.Diffuse"].SetValue(0);
            (*m_program)[L"material.Specular"].SetValue(1);
            (*m_program)[L"material.Shininess"].SetValue(32.0f);

            glm::vec3 pointLightPositions[] =
            {
                glm::vec3(0.7f,  0.2f,  2.0f),
                glm::vec3(2.3f, -3.3f, -4.0f),
                glm::vec3(-4.0f,  2.0f, -12.0f),
                glm::vec3(0.0f,  0.0f, -3.0f)
            };

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

            //glm::vec3 lightColor(sin(m_time * 2.0f), sin(m_time * 0.7f), sin(m_time * 1.3f));
            glm::vec3 lightColor(1.0f);

            (*m_program)[L"directionalLight.Direction"].SetValue(glm::vec3(-0.2f, -1.0f, -0.3f));
            (*m_program)[L"directionalLight.Ambient"].SetValue(lightColor * glm::vec3(0.1f));
            (*m_program)[L"directionalLight.Diffuse"].SetValue(lightColor * glm::vec3(0.2f));
            (*m_program)[L"directionalLight.Specular"].SetValue(glm::vec3(1.0f));

            for (int i = 0; i < 4; i++)
            {
                std::wstringstream ss;
                ss << L"pointLights[" << i << L"]";
                std::wstring index = ss.str();

                (*m_program)[index + L".Position"].SetValue(pointLightPositions[i]);
                (*m_program)[index + L".Ambient"].SetValue(lightColor * glm::vec3(0.2f));
                (*m_program)[index + L".Diffuse"].SetValue(lightColor);
                (*m_program)[index + L".Specular"].SetValue(glm::vec3(1.0f));
                (*m_program)[index + L".Constant"].SetValue(1.0f);
                (*m_program)[index + L".Linear"].SetValue(0.35f);
                (*m_program)[index + L".Quadratic"].SetValue(0.44f);
            }

            for (int i = 0; i < 10; i++)
            {
                glm::mat4 worldMatrix(1.0f);
                worldMatrix = glm::translate(worldMatrix, cubePositions[i]);

                float angle = 20.0f * i;
                worldMatrix = glm::rotate(worldMatrix, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

                (*m_program)[L"worldMatrix"].SetValue(worldMatrix);

                Renderer::Draw(*m_vertexArray, *m_program, GL_TRIANGLES, 36);
            }

            if (m_lightProgram != nullptr)
            {
                for (int i = 0; i < 4; i++)
                {
                    (*m_lightProgram)[L"viewMatrix"].SetValue(viewMatrix);
                    (*m_lightProgram)[L"projectionMatrix"].SetValue(projectionMatrix);

                    (*m_lightProgram)[L"lightColor"].SetValue(lightColor);

                    glm::mat4 lightWorldMatrix(1.0f);
                    lightWorldMatrix = glm::translate(lightWorldMatrix, pointLightPositions[i]);
                    lightWorldMatrix = glm::scale(lightWorldMatrix, glm::vec3(0.2f));

                    (*m_lightProgram)[L"worldMatrix"].SetValue(lightWorldMatrix);

                    Renderer::Draw(*m_vertexArray, *m_lightProgram, GL_TRIANGLES, 36);
                }
            }
        }
    }

    void SimpleRenderer::Update(float deltaTime)
    {
        m_time += deltaTime;

        using namespace winrt::Windows::System;
        using namespace winrt::Windows::UI::Core;

        CoreWindow coreWindow = CoreWindow::GetForCurrentThread();

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
    }

    void SimpleRenderer::UpdateWindowSize(int width, int height)
    {
        Renderer::SetViewport(width, height);
        m_windowWidth = width;
        m_windowHeight = height;
    }
}
