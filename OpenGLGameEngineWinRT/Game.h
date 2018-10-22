#pragma once

#include "Game.g.h"
#include "SimpleRenderer.h"

namespace winrt::OpenGLGameEngineWinRT::implementation
{
    enum class OpenGLVersion
    {
        GLES_2_0,
        GLES_3_0
    };

    struct Game : GameT<Game>
    {
        std::unique_ptr<OpenGLGameEngine::SimpleRenderer> m_cubeRenderer{ nullptr };
        winrt::Windows::UI::Core::CoreWindow m_window{ nullptr };

        OpenGLVersion m_driver{ OpenGLVersion::GLES_2_0 };

        EGLDisplay m_display{ EGL_NO_DISPLAY };
        EGLContext m_context{ EGL_NO_CONTEXT };
        EGLSurface m_surface{ EGL_NO_SURFACE };

        bool m_windowClosed{};

    public:
        Game() = default;

        void Initialize();
        void Load();
        void Uninitialize();
        void Run();
        void SetWindow();

    private:
        void RecreateRender();
        void InitializeEGL();
        void CleanupEGL();
    };
}

namespace winrt::OpenGLGameEngineWinRT::factory_implementation
{
    struct Game : GameT<Game, implementation::Game>
    {
    };
}
