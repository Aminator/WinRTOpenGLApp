#include "pch.h"
#include "Renderer.h"

namespace OpenGLGameEngine
{
    void Renderer::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::ClearColor(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
    }

    void Renderer::SetViewport(int width, int height)
    {
        glViewport(0, 0, width, height);
    }
}
