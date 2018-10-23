#include "pch.h"

using namespace winrt;

using namespace OpenGLGameEngineWinRT;
using namespace Windows;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::Composition;
using namespace Windows::UI::ViewManagement;

struct App : implements<App, IFrameworkViewSource, IFrameworkView>
{
    IFrameworkView CreateView()
    {
        return *this;
    }

    void Initialize(const CoreApplicationView& applicationView)
    {
        applicationView.Activated([](CoreApplicationView sender, auto args) { sender.CoreWindow().Activate(); });
    }

    void Load(const hstring&)
    {
    }

    void Uninitialize()
    {
    }

    void Run()
    {
        Game game;
        game.Initialize();
        game.SetWindow();
        game.Load();
        game.Run();
    }

    void SetWindow(const CoreWindow&)
    {
    }

    static void ExtendViewIntoTitleBar(bool extendViewIntoTitleBar)
    {
        CoreApplication::GetCurrentView().TitleBar().ExtendViewIntoTitleBar(extendViewIntoTitleBar);

        if (extendViewIntoTitleBar)
        {
            ApplicationViewTitleBar titleBar = ApplicationView::GetForCurrentView().TitleBar();
            titleBar.ButtonBackgroundColor(Colors::Transparent());
            titleBar.ButtonInactiveBackgroundColor(Colors::Transparent());
        }
    }
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    CoreApplication::Run(make<App>());
}
