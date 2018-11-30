#include "pch.h"
#include "SurfaceAngleHelper.h"
#include <EGL/egl.h>
#include <angle_windowsstore.h>

namespace AngleHelper
{
    //template <typename T>
    //T^ to_cx(winrt::Windows::Foundation::IUnknown const& from)
    //{
    //    return safe_cast<T^>(reinterpret_cast<Platform::Object^>(winrt::get_abi(from)));
    //}

    long long SurfaceAngleHelper::CreateSurface(long long display, long long config, Windows::UI::Core::CoreWindow ^ window, long long surfaceAttribList)
    {
        Windows::Foundation::Collections::PropertySet^ surfaceProperties = ref new Windows::Foundation::Collections::PropertySet();
        surfaceProperties->Insert(ref new Platform::String(EGLNativeWindowTypeProperty), window);
        EGLNativeWindowType win = reinterpret_cast<EGLNativeWindowType>(surfaceProperties);

        return (long long)eglCreateWindowSurface((EGLDisplay)display, (EGLConfig)config, win, (const EGLint*)((void*)surfaceAttribList));
    }
}
