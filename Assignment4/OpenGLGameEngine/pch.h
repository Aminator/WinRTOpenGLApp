#pragma once

#define NOMINMAX

#ifdef _WIN32

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Graphics.Imaging.h>

#endif // _WIN32

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <pplawait.h>
#include <angle_windowsstore.h>
#include <sstream>

#define GL_GLEXT_PROTOTYPES

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

#define glGenVertexArrays glGenVertexArraysOES
#define glBindVertexArray glBindVertexArrayOES
#define glDeleteVertexArrays glDeleteVertexArraysOES

#define glDrawBuffers glDrawBuffersEXT
#define GL_DRAW_FRAMEBUFFER GL_DRAW_FRAMEBUFFER_ANGLE

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::steady_clock::time_point TimePoint;
