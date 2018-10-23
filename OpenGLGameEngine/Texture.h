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
        GLenum m_format{};

    public:
        Texture() = default;
        Texture(const unsigned char* data, int width, int height, GLenum format);
        ~Texture() override;

        static concurrency::task<Texture> LoadAsync(std::wstring path);

        void Bind() const override;
        void Bind(GLenum textureUnit) const;
        void Unbind() const override;

    private:
        void SetUpTexture(const unsigned char* data) const;
    };
}
