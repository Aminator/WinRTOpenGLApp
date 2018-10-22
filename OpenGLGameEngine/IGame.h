#pragma once

#include "pch.h"

namespace OpenGLGameEngine
{
    class __declspec(dllexport) IGame
    {
    public:
        virtual ~IGame() {}

        virtual void Initialize() = 0;
        virtual void Load() = 0;
        virtual void Uninitialize() = 0;
        virtual void Run() = 0;
        virtual void SetWindow() = 0;
    };
}
