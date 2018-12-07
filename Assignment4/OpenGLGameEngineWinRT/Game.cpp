#include "pch.h"
#include "Game.h"
#include <winrt/AngleHelper.h>
#include <iostream>
#include <sstream>

using namespace winrt;

using namespace winrt::Windows;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Foundation::Numerics;
using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Composition;
using namespace winrt::Windows::UI::ViewManagement;

namespace winrt::OpenGLGameEngineWinRT::implementation
{
    void Game::Initialize()
    {
    }

    void Game::Load()
    {
        RecreateRender();
    }

    void Game::Uninitialize()
    {
    }

    void Game::Run()
    {
        m_window.Activate();
        CoreDispatcher dispatcher = m_window.Dispatcher();

        TimePoint previousTime = Clock::now();

        while (!m_windowClosed)
        {
            dispatcher.ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

            EGLint panelWidth = 0;
            EGLint panelHeight = 0;

            eglQuerySurface(m_display, m_surface, EGL_WIDTH, &panelWidth);
            eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &panelHeight);

            m_cubeRenderer->UpdateWindowSize(panelWidth, panelHeight);

            TimePoint currentTime = Clock::now();
            std::chrono::duration<float> duration = currentTime - previousTime;
            previousTime = currentTime;

            float deltaTime = duration.count();

            //std::wstringstream timeString;
            //timeString << 1.0f / deltaTime << std::endl;

            //OutputDebugString(timeString.str().c_str());

            m_cubeRenderer->Update(deltaTime);
            m_cubeRenderer->Draw(deltaTime);

            eglSwapInterval(m_display, 1);

            if (!eglSwapBuffers(m_display, m_surface))
            {
                m_cubeRenderer.reset(nullptr);
                CleanupEGL();

                InitializeEGL();
                RecreateRender();
            }
        }

        CleanupEGL();
    }

    void Game::SetWindow()
    {
        m_window = CoreWindow::GetForCurrentThread();

        CoreApplication::GetCurrentView().TitleBar().ExtendViewIntoTitleBar(true);

        ApplicationViewTitleBar titleBar = ApplicationView::GetForCurrentView().TitleBar();
        titleBar.ButtonBackgroundColor(Colors::Transparent());
        titleBar.ButtonInactiveBackgroundColor(Colors::Transparent());

        m_window.Closed([&](const CoreWindow&, const CoreWindowEventArgs&)
        {
            m_windowClosed = true;
            CleanupEGL();
        });

        InitializeEGL();
    }

    void Game::RecreateRender()
    {
        if (m_cubeRenderer == nullptr)
        {
            m_cubeRenderer.reset(new OpenGLGameEngine::SimpleRenderer());
            m_cubeRenderer->LoadContentAsync();
        }
    }

    void Game::InitializeEGL()
    {
        EGLint configAttribList[] =
        {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 8,
            EGL_STENCIL_SIZE, 8,
            EGL_SAMPLE_BUFFERS, 0,
            EGL_NONE
        };

        EGLint surfaceAttribList[] =
        {
            EGL_NONE, EGL_NONE
        };

        EGLint numConfigs = 0;
        EGLint majorVersion = 1;
        EGLint minorVersion;

        if (m_driver == OpenGLVersion::GLES_2_0)
        {
            minorVersion = 0;
        }
        else
        {
            minorVersion = 5;
        }

        EGLDisplay display = EGL_NO_DISPLAY;
        EGLContext context = EGL_NO_CONTEXT;
        EGLSurface surface = EGL_NO_SURFACE;
        EGLConfig config = nullptr;
        EGLint contextAttribs[3];

        if (m_driver == OpenGLVersion::GLES_2_0)
        {
            contextAttribs[0] = EGL_CONTEXT_CLIENT_VERSION;
            contextAttribs[1] = 2;
            contextAttribs[2] = EGL_NONE;
        }
        else
        {
            contextAttribs[0] = EGL_CONTEXT_CLIENT_VERSION;
            contextAttribs[1] = 3;
            contextAttribs[2] = EGL_NONE;
        }

        const EGLint displayAttributes[] =
        {
            /*EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
            EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE, 9,
            EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE, 3,
            EGL_NONE,*/
            // These are the default display attributes, used to request ANGLE's D3D11 renderer.
            // eglInitialize will only succeed with these attributes if the hardware supports D3D11 Feature Level 10_0+.
            EGL_PLATFORM_ANGLE_TYPE_ANGLE,
            EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,

            // EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER is an optimization that can have large performance benefits on mobile devices.
            // Its syntax is subject to change, though. Please update your Visual Studio templates if you experience compilation issues with it.
            EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER,
            EGL_TRUE,

            // EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE is an option that enables ANGLE to automatically call
            // the IDXGIDevice3::Trim method on behalf of the application when it gets suspended.
            // Calling IDXGIDevice3::Trim when an application is suspended is a Windows Store application certification requirement.
            EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE,
            EGL_TRUE,
            EGL_NONE,
        };

        PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));

        if (!eglGetPlatformDisplayEXT)
        {
            throw std::exception("Failed to get function eglGetPlatformDisplayEXT");
        }

        display = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, displayAttributes);

        if (display == EGL_NO_DISPLAY)
        {
            throw std::exception("Failed to get default EGL display");
        }

        if (eglInitialize(display, &majorVersion, &minorVersion) == EGL_FALSE)
        {
            throw std::exception("Failed to initialize EGL");
        }

        if (eglGetConfigs(display, nullptr, 0, &numConfigs) == EGL_FALSE)
        {
            throw std::exception("Failed to get EGLConfig count");
        }

        if (eglChooseConfig(display, configAttribList, &config, 1, &numConfigs) == EGL_FALSE)
        {
            throw std::exception("Failed to choose first EGLConfig count");
        }

        //auto cxWindow = to_cx<::Windows::UI::Core::CoreWindow>(m_window);
        //surface = (EGLDisplay)AngleHelper::CreateSurface((long long)display, (long long)config, cxWindow, (long long)surfaceAttribList);
        surface = (EGLDisplay)AngleHelper::SurfaceAngleHelper::CreateSurface((long long)display, (long long)config, m_window, (long long)surfaceAttribList);
        if (surface == EGL_NO_SURFACE)
        {
            throw std::exception("Failed to create EGL fullscreen surface");
        }

        context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
        if (context == EGL_NO_CONTEXT)
        {
            throw std::exception("Failed to create EGL context");
        }

        if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
        {
            throw std::exception("Failed to make fullscreen EGLSurface current");
        }

        m_display = display;
        m_surface = surface;
        m_context = context;
    }

    void Game::CleanupEGL()
    {
        if (m_display != EGL_NO_DISPLAY && m_surface != EGL_NO_SURFACE)
        {
            eglDestroySurface(m_display, m_surface);
            m_surface = EGL_NO_SURFACE;
        }

        if (m_display != EGL_NO_DISPLAY && m_context != EGL_NO_CONTEXT)
        {
            eglDestroyContext(m_display, m_context);
            m_context = EGL_NO_CONTEXT;
        }

        if (m_display != EGL_NO_DISPLAY)
        {
            eglTerminate(m_display);
            m_display = EGL_NO_DISPLAY;
        }
    }
}
