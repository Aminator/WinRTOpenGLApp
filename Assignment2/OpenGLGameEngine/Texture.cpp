#include "pch.h"
#include "Texture.h"

namespace OpenGLGameEngine
{
    Texture::Texture(const unsigned char* data, int width, int height, GLenum format)
        : m_width(width), m_height(height), m_format(format)
    {
        glGenBuffers(1, &m_rendererId);

        Bind();
        SetUpTexture(data);
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &m_rendererId);
    }

    concurrency::task<std::shared_ptr<Texture>> Texture::LoadAsync(std::wstring path)
    {
        using namespace winrt;
        using namespace Windows::Foundation;
        using namespace Windows::Storage;
        using namespace Windows::Graphics::Imaging;

        StorageFile file = co_await StorageFile::GetFileFromApplicationUriAsync(Uri(path));

        BitmapTransform transform;
        transform.Flip(BitmapFlip::Vertical);

        BitmapDecoder decoder = co_await BitmapDecoder::CreateAsync(co_await file.OpenReadAsync());
        PixelDataProvider pixelDataProvider = co_await decoder.GetPixelDataAsync(
            decoder.BitmapPixelFormat(),
            decoder.BitmapAlphaMode(),
            transform,
            ExifOrientationMode::RespectExifOrientation,
            ColorManagementMode::DoNotColorManage);

        auto imageBuffer = pixelDataProvider.DetachPixelData();

        GLenum pixelFormat;

        switch (decoder.BitmapPixelFormat())
        {
            case BitmapPixelFormat::Rgba8:
                pixelFormat = GL_RGBA;
                break;
            case BitmapPixelFormat::Bgra8:
                pixelFormat = GL_BGRA_EXT;
                break;
            default:
                throw std::exception("This format is not supported.");
        }

        auto texture = std::make_shared<Texture>(imageBuffer.data(), decoder.OrientedPixelWidth(), decoder.OrientedPixelHeight(), pixelFormat);

        return texture;
    }

    void Texture::Bind() const
    {
        glBindTexture(GL_TEXTURE_2D, m_rendererId);
    }

    void Texture::Bind(GLenum textureUnit) const
    {
        glActiveTexture(textureUnit);
        Bind();
    }

    void Texture::Unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::SetUpTexture(const unsigned char* data) const
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_width, m_height, 0, m_format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}
