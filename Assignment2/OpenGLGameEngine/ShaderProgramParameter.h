#pragma once

#include "pch.h"

namespace OpenGLGameEngine
{
    class ShaderProgramParameter
    {
    private:
        int m_location{};
        std::wstring m_name{};
        unsigned int m_rendererId{};

    public:
        ShaderProgramParameter() = default;
        ShaderProgramParameter(int location, const std::wstring& name, unsigned int rendererId);

        int Location() const { return m_location; }
        const std::wstring& Name() const { return m_name; }
        unsigned int RendererId() const { return m_rendererId; }

        void SetValue(float param) const;
        void SetValue(int param) const;
        void SetValue(const glm::mat4& param) const;
    };
}
