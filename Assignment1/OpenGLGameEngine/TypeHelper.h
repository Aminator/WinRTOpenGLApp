#pragma once

#include "pch.h"

namespace OpenGLGameEngine
{
    class TypeHelper
    {
    public:
        template<typename T>
        static GLenum GetVertexAttribPointerType()
        {
            if (std::is_same<T, GLbyte>())
            {
                return GL_BYTE;
            }
            else if (std::is_same<T, double>())
            {
                return 0x140A;
            }
            else if (std::is_same<T, GLfloat>())
            {
                return GL_FLOAT;
            }
            else if (std::is_same<T, GLint>())
            {
                return GL_INT;
            }
            else if (std::is_same<T, GLshort>())
            {
                return GL_SHORT;
            }
            else if (std::is_same<T, GLubyte>())
            {
                return GL_UNSIGNED_BYTE;
            }
            else if (std::is_same<T, GLuint>())
            {
                return GL_UNSIGNED_INT;
            }
            else if (std::is_same<T, GLushort>())
            {
                return GL_UNSIGNED_SHORT;
            }
            else
            {
                throw std::exception("Type is not allowed.");
            }
        }

        template<typename T>
        static GLenum GetDrawElementsType()
        {
            if (std::is_same<T, GLubyte>())
            {
                return GL_UNSIGNED_BYTE;
            }
            else if (std::is_same<T, GLuint>())
            {
                return GL_UNSIGNED_INT;
            }
            else if (std::is_same<T, GLushort>())
            {
                return GL_UNSIGNED_SHORT;
            }
            else
            {
                throw std::exception("Type is not allowed.");
            }
        }
    };
}
