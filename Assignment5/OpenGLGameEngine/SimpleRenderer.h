#pragma once

#include "pch.h"
#include "Framebuffer.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace OpenGLGameEngine
{
    class SimpleRenderer
    {
    private:
        std::shared_ptr<Texture> m_texture0{ nullptr };
        std::shared_ptr<Texture> m_texture1{ nullptr };
        std::shared_ptr<Texture> m_depthBuffer{ nullptr };
		std::shared_ptr<Texture> m_lightTexture{ nullptr };

        std::shared_ptr<Framebuffer> m_framebuffer{ nullptr };

        std::unique_ptr<ShaderProgram> m_program{ nullptr };
        std::unique_ptr<ShaderProgram> m_lightProgram{ nullptr };
		std::unique_ptr<ShaderProgram> m_depthProgram{ nullptr };
		std::unique_ptr<ShaderProgram> m_frustumProgram{ nullptr };
		std::unique_ptr<ShaderProgram> m_shadowMapProgram{ nullptr };

        std::unique_ptr<VertexArray<float, unsigned int>> m_vertexArray{ nullptr };
		std::unique_ptr<VertexArray<float, unsigned int>> m_quadVertexArray{ nullptr };
		std::unique_ptr<VertexArray<float, unsigned int>> m_frustumVertexArray{ nullptr };

        std::unique_ptr<VertexBuffer<float>> m_vertexBuffer{ nullptr };
		std::unique_ptr<VertexBuffer<float>> m_quadVertexBuffer{ nullptr };
		std::unique_ptr<VertexBuffer<float>> m_frustumVertexBuffer{ nullptr };

        std::unique_ptr<IndexBuffer<unsigned int>> m_indexBuffer{ nullptr };

        int m_windowHeight{};
        int m_windowWidth{};

        float m_time{};

        glm::vec3 m_cameraPos{ 0.0f, 0.0f, 3.0f };
        glm::vec3 m_cameraFront{ 0.0f, 0.0f, -1.0f };
        glm::vec3 m_cameraUp{ 0.0f, 1.0f, 0.0f };

        glm::vec2 m_previousPosition{};
        bool m_isPositionLocked{};

        float m_yaw{ -90.0f };
        float m_pitch{};

        float m_fov{ 45.0f };

		bool m_frameBufferActive{};

		float m_constantBias{ 0.001f };
		float m_slopeScale{ 0.001f };

		glm::vec3 m_lightPosition{ 0.0f, 3.0f, 3.0f };
		glm::vec3 m_lightDirection{ 0.0f, -1.0f, -1.0f };

		float m_lightYaw{ -90.0f };
		float m_lightPitch{ -45.0f };

		float m_filterRadius{ 3.0f };
		int m_samples{ 25 };

    public:
        SimpleRenderer();

        concurrency::task<void> LoadContentAsync();
        void Draw(float deltaTime);
        void Update(float deltaTime);
        void UpdateWindowSize(int width, int height);
    };
}
