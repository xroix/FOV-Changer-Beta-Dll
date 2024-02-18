// Created by XroixHD :o
/*
* Note that this file contains a lot of temporary code for testing purposes
*/

#include "pch.h"

#include "client.h"
#include "core/module.h"
#include "core/settings.h"
#include "core/input.h"
#include "utils/mem.h"
#include "utils/proc.h"
#include "utils/Logger.h"
#include "ui/renderer.h"
#include "SDK/KeyMappings.h"


// Global core things that get "extern -ed"
//extern const std::shared_ptr<UI::Main> ui =                                 std::make_shared<UI::Main>();  // needs to be independent for debug output stuff
//extern const std::shared_ptr<GameManager> gameManager =                     std::make_shared<GameManager>();
//extern const std::shared_ptr<Module::ModuleManager> moduleManager =   std::make_shared<Module::ModuleManager>();
//extern const std::shared_ptr<Network> network =                           std::make_shared<Network>();


//////////////////////////////////////////////////////////////////////////////////////////
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::System;
using namespace winrt::Windows::UI::Notifications;
using namespace winrt::Windows::Data::Xml::Dom;


void ShowToast(std::wstring message)
{
    std::wstring const title = L"FOV Changer";
    std::wstring const logo = L"https://raw.githubusercontent.com/XroixHD/MCBE-Win10-FOV-Changer/master/res/logo-full.png";

    std::wostringstream xmlString;
    xmlString << L"<toast><visual><binding template='ToastGeneric'>" <<
        L"<text>" << title << L"</text>" <<
        L"<text>" << message << L"</text>" <<
        L"<image src='" << logo << L"'" <<
        L" placement='appLogoOverride' hint-crop='circle'/>" <<
        L"</binding></visual></toast>";

    XmlDocument toastXml;

    toastXml.LoadXml(xmlString.str().c_str());

    ToastNotificationManager::CreateToastNotifier().Show(ToastNotification(toastXml));
}

#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.h>
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::UI::ViewManagement;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml;  // Needed for Window::Current()

#include <winrt/Windows.UI.Xaml.Controls.h>
using namespace winrt::Windows::UI::Xaml::Controls;


/*
 * Source: https://github.com/windows-toolkit/WindowsCommunityToolkit/blob/rel/7.0.0/Microsoft.Toolkit.Uwp/Helpers/ColorHelper.cs
 */
winrt::Windows::UI::Color FromHsv(float hue, float saturation, float value, float aplha)
{
    if (hue < 0 || hue > 360)
    {
        return winrt::Windows::UI::Colors::Black();
    }

    float chroma = value * saturation;
    float h1 = hue / 60;
    float x = chroma * (1 - abs(fmod(h1, 2) - 1));
    float m = value - chroma;
    float r1, g1, b1;

    if (h1 < 1)
    {
        r1 = chroma;
        g1 = x;
        b1 = 0;
    } else if (h1 < 2)
    {
        r1 = x;
        g1 = chroma;
        b1 = 0;
    } else if (h1 < 3)
    {
        r1 = 0;
        g1 = chroma;
        b1 = x;
    } else if (h1 < 4)
    {
        r1 = 0;
        g1 = x;
        b1 = chroma;
    } else if (h1 < 5)
    {
        r1 = x;
        g1 = 0;
        b1 = chroma;
    } else
    {
        r1 = chroma;
        g1 = 0;
        b1 = x;
    }

    byte r = (byte)(255 * (r1 + m));
    byte g = (byte)(255 * (g1 + m));
    byte b = (byte)(255 * (b1 + m));
    byte a = (byte)(255 * aplha);

    return winrt::Windows::UI::ColorHelper::FromArgb(a, r, g, b);
}


winrt::fire_and_forget DoWorkAsync(CoreWindow window)
{
    using namespace winrt;
    using namespace std::chrono_literals;

    using Colors = Windows::UI::Colors;

    // Switch to main UI Thread
    co_await winrt::resume_foreground(window.Dispatcher());

    ApplicationViewTitleBar titlebar = ApplicationView::GetForCurrentView().TitleBar();

    // Trying to achieve rainbow title
    float hue = 0;
    auto color_white = Colors::White();

    while (true)
    {
        auto color = FromHsv(hue, 1, 1, 0.5f);
        auto color_dim = FromHsv(hue, 1, 0.8, 0.5f);

        titlebar.BackgroundColor(color);
        titlebar.ButtonBackgroundColor(color);
        titlebar.ButtonHoverBackgroundColor(color_dim);
        titlebar.ButtonPressedBackgroundColor(color_dim);
        
        hue += 0.5f;
        if (hue > 360.f)
            hue = 0;

        co_await 50ms;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////


void InjectedThread(HMODULE hModule)
{
    LOG(L"FOV Changer Client :: Press END to exit!");
    ShowToast(L"uh, that is just an test, ok?");

    // Init minhook
    if (MH_Initialize() != MH_OK)
        LOG(L"Minhook init failed!");
    else
        LOG(L"Minhook successfully initialized.");


    auto& ui = Client::GetUI();
    auto& moduleManager = Client::GetModuleManager();
    //auto& inputListener = Client::GetInputListener();

    // DirectX cant run in this thread, if I'm not mistaken
    ui.StartRenderThread();

    // Todo: settings need a rewrite, why did I use so many smart pointers, doesn't seem right
    LOG(L"Doing settings test!");
    Settings::ModuleSettings ms({
        TEXTINPUT("display_name", "Display Name"),
        GROUP("zoom_id", "Zoom", {
            INTINPUT("fov", "FOV"),
            INTINPUT("hide_hand", "Hide Hand"),
            INTINPUT("sensitivity", "Sensitivity")
        }),
        INTINPUT("test01", "Some test")
    });

    //inputListener.initGameData();
    try
    {   
        moduleManager.initGameData();
    }
    catch (...) {
        LOG(L"Failed to initGameData()");
    }
    

    ui.InvokeAsUIThreadAsync([]() {
        MessageDialog dialog(L"FOV Changer successfully initialized basic stuff. Lets hope for the best!", L"Notice");
        dialog.ShowAsync();

        // ApplicationView
        ApplicationView appView = ApplicationView::GetForCurrentView();
        appView.Title(L"FOV Changer Beta");

        // CoreWindow
        CoreWindow window = CoreApplication::MainView().CoreWindow();
        window.PointerCursor(CoreCursor{ CoreCursorType::Arrow, 1 });

        DoWorkAsync(window);
    });


    while (true)
    {
        // END
        if (GetAsyncKeyState(VK_END) & 1) // || inputListener.m_keyMappings->END)
        {
            ui.InvokeAsUIThreadAsync([]()
            {
                ApplicationView::GetForCurrentView().TryConsolidateAsync(); // Todo: Validate if this even works xD
            });
           
            break;
        }

        //if (inputListener.m_keyMappings->VK_M && inputListener.m_keyMappings->CTRL)
        //{
        //    ui.InvokeAsUIThread([]() { 
        //        CoreWindow window = CoreApplication::MainView().CoreWindow();
        //        window.IsInputEnabled(false);
        //    });
        //    OutputDebugString(L"VK_M!");
        //}

        //moduleManager->loopHotkeys();

        Sleep(5);
    }

    // Todo: Proper cleanup
    LOG(L"Freeing and exiting");

    // Uninit minhook
    if (MH_Uninitialize() != MH_OK)
        LOG(L"Minhook *un*initialization failed!");
    else
        LOG(L"Minhook was successfully removed.");

    // cleanup & eject
    FreeLibraryAndExitThread(hModule, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: 
    {
        auto thread = std::thread(InjectedThread, hModule);
        thread.detach();
        // CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)InjectedThread, hModule, 0, nullptr));
        break;
    }   
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

