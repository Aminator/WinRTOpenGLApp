#pragma once

#include "pch.h"

namespace OpenGLGameEngine
{
    class StringHelper
    {
    public:
        static const char* ToCString(const std::wstring& value)
        {
            size_t i;
            size_t size = value.size() + 1;
            char* str = new char[size];
            wcstombs_s(&i, str, size, value.c_str(), size);

            return str;
        }
    };
};
