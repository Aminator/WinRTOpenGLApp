#pragma once

#include "pch.h"
#include "GLObject.h"

namespace OpenGLGameEngine
{
    class Framebuffer : public GLObject
    {
    public:
        Framebuffer()
        {
            glGenFramebuffers(1, &m_rendererId);

            Bind();
        }

        ~Framebuffer() override
        {
            glDeleteFramebuffers(1, &m_rendererId);
        }

        void Bind() const override
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_rendererId);
        }

        void BindDepthTexture(unsigned int textureId, GLenum attachment)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, textureId, 0);
        }

        void Draw(const std::vector<GLenum>& drawBuffers)
        {
            Bind();

            glDrawBuffers(1, drawBuffers.data());

            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

            if (status != GL_FRAMEBUFFER_COMPLETE)
            {
                throw std::exception("Incomplete framebuffer");
            }

            Unbind();
        }

        void Unbind() const override
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    };
}
