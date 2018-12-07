#pragma once

#include "pch.h"
#include "GLObject.h"

namespace OpenGLGameEngine
{
    class Texture : public GLObject
    {
    private:
        int m_width{};
        int m_height{};
		GLenum m_internalFormat{};
        GLenum m_format{};
		GLenum m_type{};

    public:
        Texture(const unsigned char* data, int width, int height, GLenum internalFormat, GLenum format, GLenum type);
        ~Texture() override;

		int Width() const { return m_width; }
		int Height() const { return m_height; }

        static concurrency::task<std::shared_ptr<Texture>> LoadAsync(std::wstring path);

        void Bind() const override;
        void Bind(GLenum textureUnit) const;
        void Unbind() const override;

    private:
        void SetUpTexture(const unsigned char* data) const;
    };
}
