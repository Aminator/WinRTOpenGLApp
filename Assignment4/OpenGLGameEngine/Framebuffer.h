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
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_rendererId);
        }

        void BindDepthTexture(unsigned int textureId)
        {
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureId, 0);
        }

        void Draw()
        {
            Bind();

			std::vector<GLenum> drawBuffers = { GL_NONE };
            glDrawBuffers(1, drawBuffers.data());

            GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

            if (status != GL_FRAMEBUFFER_COMPLETE)
            {
                throw std::exception("Incomplete framebuffer");
            }

            Unbind();
        }

        void Unbind() const override
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        }
    };
}
