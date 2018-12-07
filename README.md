# Computer Graphics 2

## Prerequisites

This repo uses the Universal Windows Platform (UWP) to be able to run the
apps on every Windows 10 device that is on version 1803 or higher. Thus at
least version 10.0.17134.0 of the Windows 10 SDK is needed to build this
projects in the repo. C++ 17 is also required as everything except AngleHelper
is written in C++/WinRT which is a Windows Runtime projection that uses
standard C++ 17 instead of C++/CX to write apps and libraries.

## Project Structure

To find the solution that corresponds to an assignment, navigate to a
directory at the top level namend `Assignment` followed by the assignment
number.

In each solution there are four projects:

- AngleHelper
  - Does C++/CX interop that is currently needed as the main project uses
    C++/WinRT.
- Assignment
  - This is the actual UWP app that runs and references the
    OpenGLGameEngineWinRT project. Set this as the startup project to
    launch the app. It also contains the shaders, images and other assets in
    the `Assets` directory.
- OpenGLGameEngine
  - This is a shared project that holds the majority of the OpenGL code.
    The aim with this is to wrap the C-style OpenGL calls into classes and
    almost every OpenGL related class derives from GLObject.
- OpenGLGameEngineWinRT
  - This is as Windows Runtime component that refernces OpenGLGameEngine and
    implements the main game loop. It initializes EGL as this project uses
    Microsoft's ANGLE to translate OpenGL ES calls to Direct3D 11 ones as
    neither OpenGL nor Vulkan are supported on UWP. This library can also
    be called natively from C#, VB and even JavaScript.
