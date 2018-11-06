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

    void ShaderProgramParameter::SetValue(const glm::mat4& param) const
    {
        glUseProgram(m_rendererId);
        glUniformMatrix4fv(m_location, 1, false, glm::value_ptr(param));
    }

    void ShaderProgramParameter::SetValue(const glm::vec3 & param) const
    {
        glUseProgram(m_rendererId);
        glUniform3fv(m_location, 1, glm::value_ptr(param));
    }
}
