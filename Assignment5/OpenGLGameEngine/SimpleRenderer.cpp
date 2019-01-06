#include "pch.h"
#include "SimpleRenderer.h"
#include "Renderer.h"

namespace OpenGLGameEngine
{
	SimpleRenderer::SimpleRenderer()
	{
		glEnable(GL_CULL_FACE);

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

				bool isShiftDown = sender.GetAsyncKeyState(VirtualKey::Shift) != CoreVirtualKeyStates::None;

				float yaw = isShiftDown ? m_lightYaw : m_yaw;
				float pitch = isShiftDown ? m_lightPitch : m_pitch;

				yaw += xOffset;
				pitch += yOffset;

				if (pitch > 89.0f) pitch = 89.0f;
				if (pitch < -89.0f) pitch = -89.0f;

				glm::vec3 front;
				front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
				front.y = sin(glm::radians(pitch));
				front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

				if (isShiftDown)
				{
					m_lightYaw = yaw;
					m_lightPitch = pitch;
					m_lightDirection = glm::normalize(front);
				}
				else
				{
					m_yaw = yaw;
					m_pitch = pitch;
					m_cameraFront = glm::normalize(front);
				}

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
				switch (args.VirtualKey())
				{
				case VirtualKey::Escape:
					m_isPositionLocked = false;
					if (sender.PointerCursor() == nullptr) sender.PointerCursor(CoreCursor(CoreCursorType::Arrow, 0));
					break;
				case VirtualKey::Up:
					m_constantBias += 0.0001;
					break;
				case VirtualKey::Down:
					m_constantBias -= 0.0001;
					break;
				case VirtualKey::Left:
					m_slopeScale -= 0.0001;
					break;
				case VirtualKey::Right:
					m_slopeScale += 0.0001;
					break;
				case VirtualKey::I:
					m_filterRadius++;
					break;
				case VirtualKey::K:
					m_filterRadius--;
					break;
				case VirtualKey::J:
					if (m_samples > 0)
					{
						m_samples = (sqrt(m_samples) - 2) * (sqrt(m_samples) - 2);
					}
					break;
				case VirtualKey::L:
					if (m_samples < 64)
					{
						m_samples = (sqrt(m_samples) + 2) * (sqrt(m_samples) + 2);
					}
					break;
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
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,

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
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,


			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f
		};

		const unsigned int indices[] =
		{
			0, 1, 2
		};

		const float quadVertices[] =
		{
			-1.0, 1.0, 0.0, 1.0,
			-1.0, -1.0, 0.0, 0.0,
			1.0, 1.0, 1.0, 1.0,
			1.0, -1.0, 1.0, 0.0
		};

		const float frustumVertices[] =
		{
			-1.0, -1.0, -1.0,
			-1.0, -1.0,  1.0,
			 1.0, -1.0, -1.0,
			 1.0, -1.0,  1.0,
			-1.0,  1.0, -1.0,
			-1.0,  1.0,  1.0,
			 1.0,  1.0, -1.0,
			 1.0,  1.0,  1.0,
		};

		std::wstring vertexShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///Assets/VertexShader.glsl");
		std::wstring fragmentShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///Assets/FragmentShader.glsl");

		std::wstring lampVertexShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///Assets/LightVertexShader.glsl");
		std::wstring lampFragmentShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///Assets/LightFragmentShader.glsl");

		std::wstring depthVertexShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///Assets/DepthVertexShader.glsl");
		std::wstring depthFragmentShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///Assets/DepthFragmentShader.glsl");

		std::wstring frustumVertexShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///Assets/FrustumVertexShader.glsl");
		std::wstring frustumFragmentShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///Assets/FrustumFragmentShader.glsl");

		std::wstring shadowMapVertexShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///Assets/ShadowMapVertexShader.glsl");
		std::wstring shadowMapFragmentShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///Assets/ShadowMapFragmentShader.glsl");

		std::wstring gBufferVertexShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///Assets/GBufferVertexShader.glsl");
		std::wstring gBufferFragmentShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///Assets/GBufferFragmentShader.glsl");

		std::wstring lightingVertexShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///Assets/LightingVertexShader.glsl");
		std::wstring lightingFragmentShaderSource = co_await ShaderProgram::LoadShaderAsync(L"ms-appx:///Assets/LightingFragmentShader.glsl");

		m_texture0 = co_await Texture::LoadAsync(L"ms-appx:///Assets/Container2.png");
		m_texture0->Unbind();

		m_texture1 = co_await Texture::LoadAsync(L"ms-appx:///Assets/Container2_Specular.png");
		m_texture1->Unbind();

		m_lightTexture = co_await Texture::LoadAsync(L"ms-appx:///Assets/Spotlight_Texture.png");
		m_lightTexture->Unbind();

		m_depthBuffer = std::make_unique<Texture>(nullptr, m_windowWidth, m_windowHeight, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
		m_depthBuffer->Unbind();

		//m_program = std::make_unique<ShaderProgram>(vertexShaderSource, fragmentShaderSource);
		m_program = std::make_unique<ShaderProgram>(gBufferVertexShaderSource, gBufferFragmentShaderSource);
		m_lampProgram = std::make_unique<ShaderProgram>(lampVertexShaderSource, lampFragmentShaderSource);
		m_depthProgram = std::make_unique<ShaderProgram>(depthVertexShaderSource, depthFragmentShaderSource);
		m_frustumProgram = std::make_unique<ShaderProgram>(frustumVertexShaderSource, frustumFragmentShaderSource);
		m_shadowMapProgram = std::make_unique<ShaderProgram>(shadowMapVertexShaderSource, shadowMapFragmentShaderSource);
		m_lightingProgram = std::make_unique<ShaderProgram>(lightingVertexShaderSource, lightingFragmentShaderSource);

		m_indexBuffer = std::make_unique<IndexBuffer<unsigned int>>(indices, sizeof(indices) / sizeof(unsigned int));

		{
			m_vertexBuffer = std::make_unique<VertexBuffer<float>>(vertices, sizeof(vertices) / sizeof(float));

			VertexBufferLayout<float> layout;
			layout.Add(3);
			layout.Add(2);
			layout.Add(3);

			m_vertexArray = std::make_unique<VertexArray<float, unsigned int>>(layout, *m_vertexBuffer, *m_indexBuffer);

			m_vertexArray->Unbind();
			m_vertexBuffer->Unbind();
			m_indexBuffer->Unbind();
		}

		{
			m_frustumVertexBuffer = std::make_unique<VertexBuffer<float>>(frustumVertices, sizeof(frustumVertices) / sizeof(float));

			VertexBufferLayout<float> layout;
			layout.Add(3);

			m_frustumVertexArray = std::make_unique<VertexArray<float, unsigned int>>(layout, *m_frustumVertexBuffer, *m_indexBuffer);

			m_frustumVertexArray->Unbind();
			m_frustumVertexBuffer->Unbind();
			m_indexBuffer->Unbind();
		}

		{
			m_quadVertexBuffer = std::make_unique<VertexBuffer<float>>(quadVertices, sizeof(quadVertices) / sizeof(float));

			VertexBufferLayout<float> layout;
			layout.Add(2);
			layout.Add(2);

			m_quadVertexArray = std::make_unique<VertexArray<float, unsigned int>>(layout, *m_quadVertexBuffer, *m_indexBuffer);

			m_depthBuffer->Bind(GL_TEXTURE0);

			m_framebuffer = std::make_unique<Framebuffer>();
			m_framebuffer->BindDepthTexture(m_depthBuffer->RendererId(), GL_DEPTH_ATTACHMENT);

			std::vector<GLenum> drawBuffers = { GL_NONE };
			m_framebuffer->Draw(drawBuffers);

			m_framebuffer->Unbind();
			m_depthBuffer->Unbind();

			m_quadVertexArray->Unbind();
			m_quadVertexBuffer->Unbind();
			m_indexBuffer->Unbind();
		}

		{
			std::vector<GLenum> drawBuffers = { GL_COLOR_ATTACHMENT0 };

			m_gNormal = std::make_unique<Texture>(nullptr, m_windowWidth, m_windowHeight, GL_RGB, GL_RGB, GL_FLOAT);
			m_gNormalBuffer = std::make_unique<Framebuffer>();
			m_gNormalBuffer->BindDepthTexture(m_gNormal->RendererId(), GL_COLOR_ATTACHMENT0);
			m_gNormalBuffer->Draw(drawBuffers);

			m_gNormalBuffer->Bind();
			m_gNormalBuffer->BindDepthTexture(m_depthBuffer->RendererId(), GL_DEPTH_ATTACHMENT);
			m_framebuffer->Draw({ GL_NONE });

			m_gColorSpec = std::make_unique<Texture>(nullptr, m_windowWidth, m_windowHeight, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
			m_gColorSpecBuffer = std::make_unique<Framebuffer>();
			m_gColorSpecBuffer->BindDepthTexture(m_gColorSpec->RendererId(), GL_COLOR_ATTACHMENT0);
			m_gColorSpecBuffer->Draw(drawBuffers);

			m_gColorSpecBuffer->Bind();
			m_gColorSpecBuffer->BindDepthTexture(m_depthBuffer->RendererId(), GL_DEPTH_ATTACHMENT);
			m_framebuffer->Draw({ GL_NONE });
		}

		auto result = glGetError();

		m_frameBufferActive = true;
	}

	void SimpleRenderer::Draw(float deltaTime)
	{
		if (!m_frameBufferActive) return;

		{
			m_framebuffer->Unbind();

			Renderer::ClearColor(0.2f, 0.1f, 0.6f, 1.0f);
			Renderer::Clear();

			glEnable(GL_DEPTH_TEST);
		}

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

			glm::mat4 lightViewMatrix = glm::lookAt(m_lightPosition, m_lightPosition + m_lightDirection, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 lightProjectionMatrix = glm::perspective(glm::radians(25.0f), 512.0f / 512.0f, 0.1f, 100.0f);

			(*m_program)[L"material.Diffuse"].SetValue(0);
			(*m_program)[L"material.Specular"].SetValue(1);
			(*m_program)[L"material.Shininess"].SetValue(32.0f);

			glm::vec3 lightPositions[] =
			{
				glm::vec3(0.7f,  0.2f,  2.0f),
				glm::vec3(2.3f, -3.3f, -4.0f),
				glm::vec3(-4.0f,  2.0f, -12.0f),
				glm::vec3(0.7f,  0.2f,  2.0f),
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

			ShaderProgram* currentProgram = m_program.get();

			(*currentProgram)[L"viewMatrix"].SetValue(viewMatrix);
			(*currentProgram)[L"projectionMatrix"].SetValue(projectionMatrix);

			Renderer::SetViewport(m_gNormal->Width(), m_gNormal->Height());

			Framebuffer* currentFramebuffer = nullptr;

			for (int j = 0; j < 4; j++)
			{
				switch (j)
				{
				case 0:
					currentFramebuffer = m_framebuffer.get();
					break;
				case 1:
					currentFramebuffer = m_gNormalBuffer.get();
					break;
				case 2:
					currentFramebuffer = m_gColorSpecBuffer.get();
					break;
				}

				if (j == 3)
				{
					currentFramebuffer->Unbind();
				}
				else
				{
					currentFramebuffer->Bind();
				}

				glEnable(GL_DEPTH_TEST);

				Renderer::Clear();

				(*currentProgram)[L"bufferType"].SetValue(j);

				for (int i = 0; i < 10; i++)
				{
					glm::mat4 worldMatrix(1.0f);
					worldMatrix = glm::translate(worldMatrix, cubePositions[i]);

					float angle = 20.0f * i;
					worldMatrix = glm::rotate(worldMatrix, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

					(*currentProgram)[L"worldMatrix"].SetValue(worldMatrix);

					Renderer::Draw(*m_vertexArray, *currentProgram, GL_TRIANGLES, 72);
				}

				auto result = glGetError();
			}

			currentFramebuffer->Unbind();

			glm::vec3 lightColor(1.0f);

			const int numLights = 4;

			glDisable(GL_DEPTH_TEST);

			if (m_lightingProgram != nullptr)
			{
				(*m_lightingProgram)[L"viewPosition"].SetValue(m_cameraPos);

				(*m_lightingProgram)[L"viewMatrixInv"].SetValue(glm::inverse(viewMatrix));
				(*m_lightingProgram)[L"projectionMatrixInv"].SetValue(glm::inverse(projectionMatrix));

				for (int i = 0; i < numLights; i++)
				{
					std::wstringstream ss;
					ss << L"lights[" << i << L"]";
					std::wstring index = ss.str();

					(*m_lightingProgram)[index + L".Position"].SetValue(m_lightPosition);
					(*m_lightingProgram)[index + L".Color"].SetValue(lightColor);
				}

				m_depthBuffer->Bind(GL_TEXTURE0);
				m_gNormal->Bind(GL_TEXTURE1);
				m_gColorSpec->Bind(GL_TEXTURE2);

				(*m_lightingProgram)[L"gDepth"].SetValue(0);
				(*m_lightingProgram)[L"gNormal"].SetValue(1);
				(*m_lightingProgram)[L"gAlbedoSpec"].SetValue(2);

				Renderer::Draw(*m_quadVertexArray, *m_lightingProgram, GL_TRIANGLE_STRIP, 4);
			}

			Renderer::SetViewport(512, 512);

			if (m_depthProgram != nullptr)
			{
				m_depthBuffer->Bind(GL_TEXTURE0);

				(*m_depthProgram)[L"depthTexture"].SetValue(0);
				(*m_depthProgram)[L"zNear"].SetValue(0.1f);
				(*m_depthProgram)[L"zFar"].SetValue(100.0f);

				Renderer::Draw(*m_quadVertexArray, *m_depthProgram, GL_TRIANGLE_STRIP, 4);
			}

			Renderer::SetViewport(m_windowWidth, m_windowHeight);

			glEnable(GL_DEPTH_TEST);

			if (m_lampProgram != nullptr)
			{
				for (int i = 0; i < numLights; i++)
				{
					(*m_lampProgram)[L"viewMatrix"].SetValue(viewMatrix);
					(*m_lampProgram)[L"projectionMatrix"].SetValue(projectionMatrix);

					(*m_lampProgram)[L"lightColor"].SetValue(lightColor);

					glm::mat4 lightWorldMatrix(1.0f);
					lightWorldMatrix = glm::translate(lightWorldMatrix, lightPositions[i]);
					lightWorldMatrix = glm::scale(lightWorldMatrix, glm::vec3(0.2f));

					(*m_lampProgram)[L"worldMatrix"].SetValue(lightWorldMatrix);

					Renderer::Draw(*m_vertexArray, *m_lampProgram, GL_TRIANGLES, 36);
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

		bool isShiftDown = coreWindow.GetAsyncKeyState(VirtualKey::Shift) != CoreVirtualKeyStates::None;

		glm::vec3 position = isShiftDown ? m_lightPosition : m_cameraPos;
		glm::vec3 direction = isShiftDown ? m_lightDirection : m_cameraFront;

		if (coreWindow.GetAsyncKeyState(VirtualKey::W) != CoreVirtualKeyStates::None)
		{
			position += cameraSpeed * direction;
		}

		if (coreWindow.GetAsyncKeyState(VirtualKey::S) != CoreVirtualKeyStates::None)
		{
			position -= cameraSpeed * direction;
		}

		if (coreWindow.GetAsyncKeyState(VirtualKey::A) != CoreVirtualKeyStates::None)
		{
			position -= glm::normalize(glm::cross(direction, m_cameraUp)) * cameraSpeed;
		}

		if (coreWindow.GetAsyncKeyState(VirtualKey::D) != CoreVirtualKeyStates::None)
		{
			position += glm::normalize(glm::cross(direction, m_cameraUp)) * cameraSpeed;
		}

		if (isShiftDown)
		{
			m_lightPosition = position;
		}
		else
		{
			m_cameraPos = position;
		}
	}

	void SimpleRenderer::UpdateWindowSize(int width, int height)
	{
		Renderer::SetViewport(width, height);
		m_windowWidth = width;
		m_windowHeight = height;
	}
}
