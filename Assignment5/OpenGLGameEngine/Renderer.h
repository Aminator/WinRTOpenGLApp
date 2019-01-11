#pragma once

#include "pch.h"
#include "IndexBuffer.h"
#include "ShaderProgram.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

namespace OpenGLGameEngine
{
    class Renderer
    {
    public:
        static void Clear();
		static void Clear(GLbitfield mask);
        static void ClearColor(float r, float g, float b, float a);

        template<typename T, typename S>
        static void Draw(const VertexArray<T, S>& vertexArray, const ShaderProgram& program, GLenum primitiveType, int count)
        {
            vertexArray.Bind();
            program.Bind();

            glDrawArrays(primitiveType, 0, count);
        }

        template<typename T, typename S>
        static void DrawIndexed(const VertexArray<T, S>& vertexArray, const ShaderProgram& program, GLenum primitiveType)
        {
            vertexArray.Bind();
            program.Bind();

            glDrawElements(primitiveType, vertexArray.IndexCount(), TypeHelper::GetDrawElementsType<S>(), 0);
        }

        static void SetViewport(int width, int height);
    };
}
