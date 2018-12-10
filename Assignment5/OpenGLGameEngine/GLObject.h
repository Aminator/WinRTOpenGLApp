#pragma once

#include "pch.h"

namespace OpenGLGameEngine
{
    class GLObject
    {
    protected:
        unsigned int m_rendererId{};

    public:
        virtual ~GLObject() = default;

        int RendererId() const { return m_rendererId; }

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
    };
}
