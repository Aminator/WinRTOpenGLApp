#include "pch.h"
#include "ShaderProgram.h"
#include "StringHelper.h"

namespace OpenGLGameEngine
{
    ShaderProgram::ShaderProgram(const std::wstring& vertexShader, const std::wstring& fragmentShader)
        : m_vertexShader(vertexShader), m_fragmentShader(fragmentShader)
    {
        m_rendererId = CreateShaderProgram(vertexShader, fragmentShader);

        Bind();
    }

    ShaderProgram::~ShaderProgram()
    {
        glDeleteProgram(m_rendererId);
    }

    ShaderProgramParameter ShaderProgram::operator[](const std::wstring& name)
    {
        return GetUniformLocation(name);
    }

    concurrency::task<std::wstring> ShaderProgram::LoadShaderAsync(std::wstring path)
    {
#ifdef IS_UWP
        using namespace winrt;
        using namespace Windows::Foundation;
        using namespace Windows::Storage;

        StorageFile file = co_await StorageFile::GetFileFromApplicationUriAsync(Uri(path));
        return std::wstring(co_await FileIO::ReadTextAsync(file));
#else
        std::ifstream stream(path);
        std::streampos size = stream.tellg();
        char* memblock = new char[size];

        stream.seekg(0, std::ios::beg);
        stream.read(memblock, size);
        stream.close();

        size_t i;
        wchar_t* str = new wchar_t[size];
        mbstowcs_s(&i, str, size, memblock, size);

        return std::wstring(str);
#endif
    }

    void ShaderProgram::Bind() const
    {
        glUseProgram(m_rendererId);
    }

    void ShaderProgram::Unbind() const
    {
        glUseProgram(0);
    }

    unsigned int ShaderProgram::CompileShader(GLenum shaderType, const std::wstring& shader)
    {
        const char* str = StringHelper::ToCString(shader);

        unsigned int shaderId = glCreateShader(shaderType);
        glShaderSource(shaderId, 1, &str, nullptr);
        glCompileShader(shaderId);

        return shaderId;
    }

    unsigned int ShaderProgram::CreateShaderProgram(const std::wstring& vertexShader, const std::wstring& fragmentShader)
    {
        unsigned int programId = glCreateProgram();
        unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
        unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

        int success;
        char infoLog[512];
        glGetShaderiv(vs, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(vs, 512, NULL, infoLog);
            throw std::exception(infoLog);
        }

        glGetShaderiv(fs, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(fs, 512, NULL, infoLog);
            throw std::exception(infoLog);
        }

        glAttachShader(programId, vs);
        glAttachShader(programId, fs);
        glLinkProgram(programId);
        glValidateProgram(programId);

        glGetProgramiv(programId, GL_LINK_STATUS, &success);

        if (!success)
        {
            glGetProgramInfoLog(programId, 512, NULL, infoLog);
            throw std::exception(infoLog);
        }

        glDeleteShader(vs);
        glDeleteShader(fs);

        return programId;
    }

    ShaderProgramParameter ShaderProgram::GetUniformLocation(const std::wstring& name)
    {
        if (m_uniformLocationCache.count(name))
        {
            return m_uniformLocationCache[name];
        }

        int location = glGetUniformLocation(m_rendererId, StringHelper::ToCString(name));

        if (location == -1)
        {
            throw std::exception("Warning: Uniform doesn't exist!");
        }

        ShaderProgramParameter parameter(location, name, m_rendererId);
        m_uniformLocationCache.insert(std::make_pair(name, parameter));

        return parameter;
    }
}
