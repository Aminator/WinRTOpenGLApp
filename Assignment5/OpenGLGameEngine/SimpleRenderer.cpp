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

		const float sphereVertices[] =
		{
			0.38268343, 0.92387953, 0.0000000e+0,
			0.35355339, 0.92387953, 0.14644661,
			0.27059805, 0.92387953, 0.27059805,
			0.14644661, 0.92387953, 0.35355339,
			2.3432602e-17, 0.92387953, 0.38268343,
			-0.14644661, 0.92387953, 0.35355339,
			-0.27059805, 0.92387953, 0.27059805,
			-0.35355339, 0.92387953, 0.14644661,
			-0.38268343, 0.92387953, 4.6865204e-17,
			-0.35355339, 0.92387953, -0.14644661,
			-0.27059805, 0.92387953, -0.27059805,
			-0.14644661, 0.92387953, -0.35355339,
			-7.0297806e-17, 0.92387953, -0.38268343,
			0.14644661, 0.92387953, -0.35355339,
			0.27059805, 0.92387953, -0.27059805,
			0.35355339, 0.92387953, -0.14644661,
			0.70710678, 0.70710678, 0.0000000e+0,
			0.65328148, 0.70710678, 0.27059805,
			0.50000000, 0.70710678, 0.50000000,
			0.27059805, 0.70710678, 0.65328148,
			4.3297803e-17, 0.70710678, 0.70710678,
			-0.27059805, 0.70710678, 0.65328148,
			-0.50000000, 0.70710678, 0.50000000,
			-0.65328148, 0.70710678, 0.27059805,
			-0.70710678, 0.70710678, 8.6595606e-17,
			-0.65328148, 0.70710678, -0.27059805,
			-0.50000000, 0.70710678, -0.50000000,
			-0.27059805, 0.70710678, -0.65328148,
			-1.2989341e-16, 0.70710678, -0.70710678,
			0.27059805, 0.70710678, -0.65328148,
			0.50000000, 0.70710678, -0.50000000,
			0.65328148, 0.70710678, -0.27059805,
			0.92387953, 0.38268343, 0.0000000e+0,
			0.85355339, 0.38268343, 0.35355339,
			0.65328148, 0.38268343, 0.65328148,
			0.35355339, 0.38268343, 0.85355339,
			5.6571306e-17, 0.38268343, 0.92387953,
			-0.35355339, 0.38268343, 0.85355339,
			-0.65328148, 0.38268343, 0.65328148,
			-0.85355339, 0.38268343, 0.35355339,
			-0.92387953, 0.38268343, 1.1314261e-16,
			-0.85355339, 0.38268343, -0.35355339,
			-0.65328148, 0.38268343, -0.65328148,
			-0.35355339, 0.38268343, -0.85355339,
			-1.6971392e-16, 0.38268343, -0.92387953,
			0.35355339, 0.38268343, -0.85355339,
			0.65328148, 0.38268343, -0.65328148,
			0.85355339, 0.38268343, -0.35355339,
			1.00000000, 6.1232340e-17, 0.0000000e+0,
			0.92387953, 6.1232340e-17, 0.38268343,
			0.70710678, 6.1232340e-17, 0.70710678,
			0.38268343, 6.1232340e-17, 0.92387953,
			6.1232340e-17, 6.1232340e-17, 1.00000000,
			-0.38268343, 6.1232340e-17, 0.92387953,
			-0.70710678, 6.1232340e-17, 0.70710678,
			-0.92387953, 6.1232340e-17, 0.38268343,
			-1.00000000, 6.1232340e-17, 1.2246468e-1,
			-0.92387953, 6.1232340e-17, -0.38268343,
			-0.70710678, 6.1232340e-17, -0.70710678,
			-0.38268343, 6.1232340e-17, -0.92387953,
			-1.8369702e-16, 6.1232340e-17, -1.000000,
			0.38268343, 6.1232340e-17, -0.92387953,
			0.70710678, 6.1232340e-17, -0.70710678,
			0.92387953, 6.1232340e-17, -0.38268343,
			0.92387953, -0.38268343, 0.0000000e+0,
			0.85355339, -0.38268343, 0.35355339,
			0.65328148, -0.38268343, 0.65328148,
			0.35355339, -0.38268343, 0.85355339,
			5.6571306e-17, -0.38268343, 0.92387953,
			-0.35355339, -0.38268343, 0.85355339,
			-0.65328148, -0.38268343, 0.65328148,
			-0.85355339, -0.38268343, 0.35355339,
			-0.92387953, -0.38268343, 1.1314261e-16,
			-0.85355339, -0.38268343, -0.35355339,
			-0.65328148, -0.38268343, -0.65328148,
			-0.35355339, -0.38268343, -0.85355339,
			-1.6971392e-16, -0.38268343, -0.92387953,
			0.35355339, -0.38268343, -0.85355339,
			0.65328148, -0.38268343, -0.65328148,
			0.85355339, -0.38268343, -0.35355339,
			0.70710678, -0.70710678, 0.0000000e+0,
			0.65328148, -0.70710678, 0.27059805,
			0.50000000, -0.70710678, 0.50000000,
			0.27059805, -0.70710678, 0.65328148,
			4.3297803e-17, -0.70710678, 0.70710678,
			-0.27059805, -0.70710678, 0.65328148,
			-0.50000000, -0.70710678, 0.50000000,
			-0.65328148, -0.70710678, 0.27059805,
			-0.70710678, -0.70710678, 8.6595606e-17,
			-0.65328148, -0.70710678, -0.27059805,
			-0.50000000, -0.70710678, -0.50000000,
			-0.27059805, -0.70710678, -0.65328148,
			-1.2989341e-16, -0.70710678, -0.70710678,
			0.27059805, -0.70710678, -0.65328148,
			0.50000000, -0.70710678, -0.50000000,
			0.65328148, -0.70710678, -0.27059805,
			0.38268343, -0.92387953, 0.0000000e+0,
			0.35355339, -0.92387953, 0.14644661,
			0.27059805, -0.92387953, 0.27059805,
			0.14644661, -0.92387953, 0.35355339,
			2.3432602e-17, -0.92387953, 0.38268343,
			-0.14644661, -0.92387953, 0.35355339,
			-0.27059805, -0.92387953, 0.27059805,
			-0.35355339, -0.92387953, 0.14644661,
			-0.38268343, -0.92387953, 4.6865204e-17,
			-0.35355339, -0.92387953, -0.14644661,
			-0.27059805, -0.92387953, -0.27059805,
			-0.14644661, -0.92387953, -0.35355339,
			-7.0297806e-17, -0.92387953, -0.38268343,
			0.14644661, -0.92387953, -0.35355339,
			0.27059805, -0.92387953, -0.27059805,
			0.35355339, -0.92387953, -0.14644661,
			0.0000000e+0, 1.00000000, 0.0000000e+0,
			0.0000000e+0, -1.00000000, 0.0000000e+0
		};

		unsigned int sphereIndices[] =
		{
			1, 18, 17,
			1, 32, 16,
			1, 113, 2,
			2, 18, 1,
			2, 113, 3,
			3, 18, 2,
			3, 20, 19,
			3, 113, 4,
			4, 20, 3,
			4, 113, 5,
			5, 20, 4,
			5, 22, 21,
			5, 113, 6,
			6, 22, 5,
			6, 113, 7,
			7, 22, 6,
			7, 24, 23,
			7, 113, 8,
			8, 24, 7,
			8, 113, 9,
			9, 24, 8,
			9, 26, 25,
			9, 113, 10,
			10, 26, 9,
			10, 113, 11,
			11, 26, 10,
			11, 28, 27,
			11, 113, 12,
			12, 28, 11,
			12, 113, 13,
			13, 28, 12,
			13, 30, 29,
			13, 113, 14,
			14, 30, 13,
			14, 113, 15,
			15, 30, 14,
			15, 32, 31,
			15, 113, 16,
			16, 32, 15,
			16, 113, 1,
			17, 32, 1,
			17, 33, 32,
			18, 33, 17,
			18, 35, 34,
			19, 18, 3,
			19, 35, 18,
			20, 35, 19,
			20, 37, 36,
			21, 20, 5,
			21, 37, 20,
			22, 37, 21,
			22, 39, 38,
			23, 22, 7,
			23, 39, 22,
			24, 39, 23,
			24, 41, 40,
			25, 24, 9,
			25, 41, 24,
			26, 41, 25,
			26, 43, 42,
			27, 26, 11,
			27, 43, 26,
			28, 43, 27,
			28, 45, 44,
			29, 28, 13,
			29, 45, 28,
			30, 45, 29,
			30, 47, 46,
			31, 30, 15,
			31, 47, 30,
			32, 33, 48,
			32, 47, 31,
			33, 50, 49,
			33, 64, 48,
			34, 33, 18,
			34, 50, 33,
			35, 50, 34,
			35, 52, 51,
			36, 35, 20,
			36, 52, 35,
			37, 52, 36,
			37, 54, 53,
			38, 37, 22,
			38, 54, 37,
			39, 54, 38,
			39, 56, 55,
			40, 39, 24,
			40, 56, 39,
			41, 56, 40,
			41, 58, 57,
			42, 41, 26,
			42, 58, 41,
			43, 58, 42,
			43, 60, 59,
			44, 43, 28,
			44, 60, 43,
			45, 60, 44,
			45, 62, 61,
			46, 45, 30,
			46, 62, 45,
			47, 62, 46,
			47, 64, 63,
			48, 47, 32,
			48, 64, 47,
			49, 64, 33,
			49, 65, 64,
			50, 65, 49,
			50, 67, 66,
			51, 50, 35,
			51, 67, 50,
			52, 67, 51,
			52, 69, 68,
			53, 52, 37,
			53, 69, 52,
			54, 69, 53,
			54, 71, 70,
			55, 54, 39,
			55, 71, 54,
			56, 71, 55,
			56, 73, 72,
			57, 56, 41,
			57, 73, 56,
			58, 73, 57,
			58, 75, 74,
			59, 58, 43,
			59, 75, 58,
			60, 75, 59,
			60, 77, 76,
			61, 60, 45,
			61, 77, 60,
			62, 77, 61,
			62, 79, 78,
			63, 62, 47,
			63, 79, 62,
			64, 65, 80,
			64, 79, 63,
			65, 82, 81,
			65, 96, 80,
			66, 65, 50,
			66, 82, 65,
			67, 82, 66,
			67, 84, 83,
			68, 67, 52,
			68, 84, 67,
			69, 84, 68,
			69, 86, 85,
			70, 69, 54,
			70, 86, 69,
			71, 86, 70,
			71, 88, 87,
			72, 71, 56,
			72, 88, 71,
			73, 88, 72,
			73, 90, 89,
			74, 73, 58,
			74, 90, 73,
			75, 90, 74,
			75, 92, 91,
			76, 75, 60,
			76, 92, 75,
			77, 92, 76,
			77, 94, 93,
			78, 77, 62,
			78, 94, 77,
			79, 94, 78,
			79, 96, 95,
			80, 79, 64,
			80, 96, 79,
			81, 96, 65,
			81, 97, 96,
			82, 97, 81,
			82, 99, 98,
			83, 82, 67,
			83, 99, 82,
			84, 99, 83,
			84, 101, 100,
			85, 84, 69,
			85, 101, 84,
			86, 101, 85,
			86, 103, 102,
			87, 86, 71,
			87, 103, 86,
			88, 103, 87,
			88, 105, 104,
			89, 88, 73,
			89, 105, 88,
			90, 105, 89,
			90, 107, 106,
			91, 90, 75,
			91, 107, 90,
			92, 107, 91,
			92, 109, 108,
			93, 92, 77,
			93, 109, 92,
			94, 109, 93,
			94, 111, 110,
			95, 94, 79,
			95, 111, 94,
			96, 97, 112,
			96, 111, 95,
			97, 114, 112,
			98, 97, 82,
			98, 114, 97,
			99, 114, 98,
			100, 99, 84,
			100, 114, 99,
			101, 114, 100,
			102, 101, 86,
			102, 114, 101,
			103, 114, 102,
			104, 103, 88,
			104, 114, 103,
			105, 114, 104,
			106, 105, 90,
			106, 114, 105,
			107, 114, 106,
			108, 107, 92,
			108, 114, 107,
			109, 114, 108,
			110, 109, 94,
			110, 114, 109,
			111, 114, 110,
			112, 111, 96,
			112, 114, 111
		};

		for (int i = 0; i < 672; i++)
		{
			sphereIndices[i]--;
		}

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
			m_sphereIndexBuffer = std::make_unique<IndexBuffer<unsigned int>>(sphereIndices, sizeof(sphereIndices) / sizeof(unsigned int));
			m_sphereVertexBuffer = std::make_unique<VertexBuffer<float>>(sphereVertices, sizeof(sphereVertices) / sizeof(float));

			VertexBufferLayout<float> layout;
			layout.Add(3);

			m_sphereVertexArray = std::make_unique<VertexArray<float, unsigned int>>(layout, *m_sphereVertexBuffer, *m_sphereIndexBuffer);

			m_sphereVertexArray->Unbind();
			m_sphereVertexBuffer->Unbind();
			m_sphereIndexBuffer->Unbind();
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

			//Renderer::ClearColor(0.2f, 0.1f, 0.6f, 1.0f);
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
				glm::vec3(-1.3f,  1.0f, -1.5f),
				glm::vec3(0.0f,  0.0f,  10.0f),
				glm::vec3(2.0f,  5.0f, -5.0f),
				glm::vec3(-1.5f, -2.2f, 7.5f),
				glm::vec3(-3.8f, -2.0f, -2.3f),
				glm::vec3(2.4f, -0.4f, 6.5f),
				glm::vec3(-1.7f,  3.0f, 2.5f),
				glm::vec3(1.3f, -2.0f, 7.5f),
				glm::vec3(1.5f,  2.0f, 7.5f),
				glm::vec3(1.5f,  0.2f, 8.5f),
				glm::vec3(-1.3f,  1.0f, 8.5f),
			};

			ShaderProgram* currentProgram = m_program.get();

			(*currentProgram)[L"viewMatrix"].SetValue(viewMatrix);
			(*currentProgram)[L"projectionMatrix"].SetValue(projectionMatrix);

			Renderer::SetViewport(m_gNormal->Width(), m_gNormal->Height());

			Framebuffer* currentFramebuffer = nullptr;

			for (int j = 1; j < 4; j++)
			{
				switch (j)
				{
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

				for (int i = 0; i < 20; i++)
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
			Renderer::Clear(GL_COLOR_BUFFER_BIT);

			const int numLights = 6;

			std::vector<glm::vec3> lightPositions;
			std::vector<glm::vec3> lightColors;
			srand(13);

			for (unsigned int i = 0; i < numLights; i++)
			{
				// Calculate slightly random offsets
				float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
				float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
				float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;

				lightPositions.push_back(glm::vec3(xPos, yPos, zPos));

				// Also calculate random color
				float rColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
				float gColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
				float bColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0

				lightColors.push_back(glm::vec3(rColor, gColor, bColor));
			}

			glCullFace(GL_FRONT);
			glDepthFunc(GL_GREATER);

			glDepthMask(false);

			if (m_lightingProgram != nullptr)
			{
				(*m_lightingProgram)[L"numLights"].SetValue(numLights);
				(*m_lightingProgram)[L"viewPosition"].SetValue(m_cameraPos);

				(*m_lightingProgram)[L"viewMatrixInv"].SetValue(glm::inverse(viewMatrix));
				(*m_lightingProgram)[L"projectionMatrixInv"].SetValue(glm::inverse(projectionMatrix));

				float constant = 1.0f;
				float linear = 0.35f;
				float quadratic = 0.44f;

				for (int i = 0; i < numLights; i++)
				{
					std::wstringstream ss;
					ss << L"lights[" << i << L"]";
					std::wstring index = ss.str();

					(*m_lightingProgram)[index + L".Position"].SetValue(lightPositions[i]);
					(*m_lightingProgram)[index + L".Ambient"].SetValue(lightColors[i] * glm::vec3(0.1f));
					(*m_lightingProgram)[index + L".Diffuse"].SetValue(lightColors[i]);
					(*m_lightingProgram)[index + L".Specular"].SetValue(glm::vec3(1.0f));
					(*m_lightingProgram)[index + L".Constant"].SetValue(constant);
					(*m_lightingProgram)[index + L".Linear"].SetValue(linear);
					(*m_lightingProgram)[index + L".Quadratic"].SetValue(quadratic);
				}

				m_depthBuffer->Bind(GL_TEXTURE0);
				m_gNormal->Bind(GL_TEXTURE1);
				m_gColorSpec->Bind(GL_TEXTURE2);

				(*m_lightingProgram)[L"gDepth"].SetValue(0);
				(*m_lightingProgram)[L"gNormal"].SetValue(1);
				(*m_lightingProgram)[L"gAlbedoSpec"].SetValue(2);

				(*m_lightingProgram)[L"viewPortSize"].SetValue(glm::vec2(m_windowWidth, m_windowHeight));

				for (int i = 0; i < numLights; i++)
				{
					(*m_lightingProgram)[L"viewMatrix"].SetValue(viewMatrix);
					(*m_lightingProgram)[L"projectionMatrix"].SetValue(projectionMatrix);

					glm::mat4 lightWorldMatrix(1.0f);
					lightWorldMatrix = glm::translate(lightWorldMatrix, lightPositions[i]);

					float lightMax = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
					float radius = (-linear + std::sqrtf(linear * linear - 4.0f * quadratic * (constant - (256.0f / 5.0f) * lightMax))) / (2.0f * quadratic);

					lightWorldMatrix = glm::scale(lightWorldMatrix, glm::vec3(radius));

					(*m_lightingProgram)[L"worldMatrix"].SetValue(lightWorldMatrix);

					Renderer::DrawIndexed(*m_sphereVertexArray, *m_lightingProgram, GL_TRIANGLES);
				}
			}

			glCullFace(GL_BACK);
			glDepthFunc(GL_LESS);

			glDepthMask(true);

			//Renderer::SetViewport(512, 512);

			//if (m_depthProgram != nullptr)
			//{
			//	m_depthBuffer->Bind(GL_TEXTURE0);

			//	(*m_depthProgram)[L"depthTexture"].SetValue(0);
			//	(*m_depthProgram)[L"zNear"].SetValue(0.1f);
			//	(*m_depthProgram)[L"zFar"].SetValue(100.0f);

			//	Renderer::Draw(*m_quadVertexArray, *m_depthProgram, GL_TRIANGLE_STRIP, 4);
			//}

			//Renderer::SetViewport(m_windowWidth, m_windowHeight);

			if (m_lampProgram != nullptr)
			{
				for (int i = 0; i < numLights; i++)
				{
					(*m_lampProgram)[L"viewMatrix"].SetValue(viewMatrix);
					(*m_lampProgram)[L"projectionMatrix"].SetValue(projectionMatrix);

					(*m_lampProgram)[L"lightColor"].SetValue(lightColors[i]);

					glm::mat4 lightWorldMatrix(1.0f);
					lightWorldMatrix = glm::translate(lightWorldMatrix, lightPositions[i]);
					lightWorldMatrix = glm::scale(lightWorldMatrix, glm::vec3(0.2f));

					(*m_lampProgram)[L"worldMatrix"].SetValue(lightWorldMatrix);

					Renderer::DrawIndexed(*m_sphereVertexArray, *m_lampProgram, GL_TRIANGLES);
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
