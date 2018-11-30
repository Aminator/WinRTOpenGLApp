#pragma once

namespace AngleHelper
{
    public ref class SurfaceAngleHelper sealed
    {
    public:
        static long long CreateSurface(long long display, long long config, Windows::UI::Core::CoreWindow^ window, long long surfaceAttribList);
    };
}
