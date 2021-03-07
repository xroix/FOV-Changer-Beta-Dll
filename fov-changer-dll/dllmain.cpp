// Created by XroixHD :o

#include "pch.h"

#include "game.h"
#include "core/module.h"
#include "core/settings.h"
#include "utils/mem.h"
#include "utils/proc.h"
#include "utils/Logger.h"
#include "network/network.h"
#include "ui/renderer.h"
#include "SDK/KeyMappings.h"


// Global core things that get "extern -ed"
extern const std::shared_ptr<GameManager> gameManager =                     std::make_shared<GameManager>();
extern const std::shared_ptr<Core::Module::ModuleManager> moduleManager =   std::make_shared<Core::Module::ModuleManager>();
//extern const std::shared_ptr<Network> network =                           std::make_shared<Network>();
const std::shared_ptr<UI::Main> ui =                                        std::make_shared<UI::Main>();


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

#include <winrt/Windows.ApplicationModel.Core.h>
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

    auto views = CoreApplication::Views();

    if (Window::Current() == NULL)
        OutputDebugString(L"Window::Current() is NULL\n");

    appView.Title(L"FOV CHANGER");
}

//////////////////////////////////////////////////////////////////////////////////////////


DWORD WINAPI InjectedThread(HMODULE hModule)
{
    LOG(L"FOV Changer Client :: Press END to exit!");
    ShowToast(L"Hello world");

    // Init minhook
    if (MH_Initialize() != MH_OK)
        LOG(L"Minhook init failed!");
    else
        LOG(L"Minhook successfully initialized.");

    ui->StartRenderThread(ui);

    LOG(L"Doing settings test!");
    Core::Settings::ModuleSettings ms({
        TEXTINPUT("display_name", "Display Name"),
        GROUP("zoom_id", "Zoom", {
            INTINPUT("fov", "FOV"),
            INTINPUT("hide_hand", "Hide Hand"),
            INTINPUT("sensitivity", "Sensitivity")
        }),
        INTINPUT("test01", "Some test")
    });

    gameManager->m_inputListener->initGameData();
    //network->startThread();
    moduleManager->initGameData();

    while (true)
    {
        // END
        if (GetAsyncKeyState(VK_END) & 1 || gameManager->m_inputListener->m_keyMappings->END)
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

