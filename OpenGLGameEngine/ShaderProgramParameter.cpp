#include "pch.h"
#include "ShaderProgramParameter.h"

namespace OpenGLGameEngine
{
    ShaderProgramParameter::ShaderProgramParameter(int location, const std::wstring& name, unsigned int rendererId)
        : m_location(location), m_name(name), m_rendererId(rendererId)
    {
    }

    void ShaderProgramParameter::SetValue(float param) const
    {
        glUseProgram(m_rendererId);
        glUniform1f(m_location, param);
    }

    void ShaderProgramParameter::SetValue(int param) const
    {
        glUseProgram(m_rendererId);
        glUniform1i(m_location, param);
    }
}
