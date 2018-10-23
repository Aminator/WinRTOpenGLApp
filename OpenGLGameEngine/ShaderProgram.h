#pragma once

#include "pch.h"
#include "GLObject.h"
#include "ShaderProgramParameter.h"

namespace OpenGLGameEngine
{
    class ShaderProgram : public GLObject
    {
    private:
        std::wstring m_vertexShader{};
        std::wstring m_fragmentShader{};
        std::unordered_map<std::wstring, ShaderProgramParameter> m_uniformLocationCache{};

    public:
        ShaderProgram(const std::wstring& vertexShader, const std::wstring& fragmentShader);
        ~ShaderProgram() override;

        const std::wstring& VertexShader() const { return m_vertexShader; }
        const std::wstring& FragmentShader() const { return m_fragmentShader; }

        ShaderProgramParameter operator[](const std::wstring& name);

        static concurrency::task<std::wstring> LoadShaderAsync(std::wstring path);

        void Bind() const override;
        void Unbind() const override;

    private:
        static unsigned int CompileShader(GLenum shaderType, const std::wstring& shader);
        static unsigned int CreateShaderProgram(const std::wstring& vertexShader, const std::wstring& fragmentShader);

        ShaderProgramParameter GetUniformLocation(const std::wstring& name);
    };
}
