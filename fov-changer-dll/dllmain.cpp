// Created by XroixHD :o

#include "pch.h"

#include "module.h"
#include "game.h"
#include "settings.h"
#include "utils/mem.h"
#include "utils/proc.h"
#include "utils/Logger.h"
#include "network/network.h"
#include "ui/renderer.h"
#include "SDK/KeyListener.h"


// Global core things that get "extern -ed"
extern const std::shared_ptr<GameManager> gameManager = std::make_shared<GameManager>();
extern const std::shared_ptr<ModuleManager> moduleManager = std::make_shared<ModuleManager>();
//extern const std::shared_ptr<Network> network = std::make_shared<Network>();
const std::shared_ptr<UI::Renderer> renderer = std::make_shared<UI::Renderer>();


//////////////////////////////////////////////////////////////////////////////////////////



using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::System;
using namespace winrt::Windows::UI::Notifications;
using namespace winrt::Windows::Data::Xml::Dom;


void ShowToast(std::wstring message)
{
    std::wstring const title = L"FOV Changer";
    std::wstring const content = L"Message: \n";
    std::wstring const logo = L"https://raw.githubusercontent.com/XroixHD/MCBE-Win10-FOV-Changer/master/res/logo-full.png";

    std::wostringstream xmlString;
    xmlString << L"<toast><visual><binding template='ToastGeneric'>" <<
        L"<text>" << title << L"</text>" <<
        L"<text>" << content << message << L"</text>" <<
        L"<image src='" << logo << L"'" <<
        L" placement='appLogoOverride' hint-crop='circle'/>" <<
        L"</binding></visual></toast>";

    XmlDocument toastXml;

    toastXml.LoadXml(xmlString.str().c_str());

    ToastNotificationManager::CreateToastNotifier().Show(ToastNotification(toastXml));
}

#include <winrt/Windows.ApplicationModel.Core.h>;
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::UI::ViewManagement;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml;  // Needed for Window::Current()


IAsyncAction DoWorkAsync(CoreWindow window)
{

    // Switch to main UI Thread
    co_await winrt::resume_foreground(window.Dispatcher());

    // Setting title
    ApplicationView appView = ApplicationView::GetForCurrentView();
    appView.Title(L"Step 1");

    // App window method
    /*AppWindow newWindow = co_await AppWindow::TryCreateAsync();
    Frame newWindowContentFrame{};
    ElementCompositionPreview::SetAppWindowContent(newWindow, newWindowContentFrame);
    newWindow.TryShowAsync();*/

    // CreateView method
    //CoreApplicationView newView = CoreApplication::CreateNewView();
    /*co_await winrt::resume_foreground(newView.Dispatcher());
    Frame frame{};
    Window::Current().Activate();
    ApplicationViewSwitcher::TryShowAsStandaloneAsync(ApplicationView::GetForCurrentView().Id());*/

    auto views = CoreApplication::Views();

    if (Window::Current() == NULL)
        OutputDebugString(L"Window::Current() is NULL\n");

    appView.Title(L"FOV CHANGER");
}

void GuiTest()
{
    try
    {
        CoreApplicationView view = CoreApplication::MainView();
        CoreWindow window = view.CoreWindow();
        CoreDispatcher dispatcher = window.Dispatcher();

        winrt::agile_ref<CoreWindow> windowAgile{ view.CoreWindow() };

        //std::thread t = std::thread(UI::RenderingThread, renderer, window);
        //t.join();

        dispatcher.RunAsync(CoreDispatcherPriority::Low, [&](auto && ...)
            {
                try
                {
                    renderer->Setup(CoreApplication::MainView().CoreWindow());
                    renderer->Loop();
                    //if (Window::Current() == NULL)
                    //    OutputDebugString(L"DISPATCHER: Window::Current() is NULL\n");

                    //if (GetActiveWindow() == NULL)
                    //    OutputDebugString(L"DISPATCHER: OutputDebugString() is NULL\n");
                    //else
                    //    OutputDebugString(L"DISPATCHER: OutputDebugString() is VALID!!!!\n");


                    //Compositor compositor{};
                    //ContainerVisual containerVisual = compositor.CreateContainerVisual();
                    //auto target = compositor.CreateTargetForCurrentView();
                    //target.Root(containerVisual);

                    //// Create sprite
                    //SpriteVisual visual = compositor.CreateSpriteVisual();

                    //// Set coolor
                    //visual.Brush(compositor.CreateColorBrush({ 0xff, 0xff, 0xff, 0xff }));

                    //// Size
                    //visual.Size({ 300.0f, 300.0f });

                    //// Location
                    //visual.Offset({ 100.0f, 100.0f, 0.0f });

                    //containerVisual.Children().InsertAtTop(visual);
                }
                catch (winrt::hresult_error const& ex)
                {
                    winrt::hresult hr = ex.code(); // HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND).
                    winrt::hstring message = ex.message(); // The system cannot find the file specified.

                    LOG(L"ERROR 2:");
                    LOG(message.c_str());
                }
               
         });

        // Trying to create an additional view
        /*CoreApplicationView newView = CoreApplication::CreateNewView();

        newView.Dispatcher().RunAsync(CoreDispatcherPriority::Low, [&](auto&& ...)
            {
                Frame frame{};

                Window::Current().Content() = frame;
                Window::Current().Activate();

            });*/

        DoWorkAsync(window);

    } catch (winrt::hresult_error const& ex)
    {
        winrt::hresult hr = ex.code(); // HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND).
        winrt::hstring message = ex.message(); // The system cannot find the file specified.

        LOG(L"ERROR:");
        LOG(message.c_str());
    }
}


//////////////////////////////////////////////////////////////////////////////////////////


DWORD WINAPI InjectedThread(HMODULE hModule)
{
    LOG(L"FOV Changer Client :: Press END to exit!");
    ShowToast(L"Hello world");

    OutputDebugString(L"Modulebase: ");
    OutputDebugStringA(std::to_string(gameManager->m_moduleBase).c_str());
    OutputDebugString(L"\n");

    // Init minhook
    if (MH_Initialize() != MH_OK)
        LOG(L"Minhook init failed!");
    else
        LOG(L"Minhook successfully initialized.");


    LOG(L"Doing settings test!");
    ModuleSettings ms({
        TEXTINPUT("display_name", "Display Name"),
        GROUP("zoom_id", "Zoom", {
            INTINPUT("fov", "FOV"),
            INTINPUT("hide_hand", "Hide Hand"),
            INTINPUT("sensitivity", "Sensitivity")
        }),
        INTINPUT("test01", "Some test")
    });

    LOG(L"Doing ui tests!");
    GuiTest();

    //network->startThread();
    moduleManager->initGameData();

    while (true)
    {
        // END
        if (GetAsyncKeyState(VK_END) & 1 || gameManager->c_KeyListener->END)
        {
            Application::Current().Exit();
        }

        moduleManager->loopHotkeys();

        Sleep(5);
    }

    LOG(L"Freeing and exiting");

    // Uninit minhook
    if (MH_Uninitialize() != MH_OK)
        LOG(L"Minhook *un*initialization failed!");
    else
        LOG(L"Minhook was successfully removed.");

    // cleanup & eject
    FreeLibraryAndExitThread(hModule, 0);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)InjectedThread, hModule, 0, nullptr));
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

