#include "..\pch.h"
#include "..\utils\Logger.h"
#include "zoom.h"


extern const std::shared_ptr<GameManager> gameManager;


// Used at hooks to get Zoom class
static Zoom* g_RefZoom = nullptr;


void __fastcall hkUpCallFunc(void* param_1, uint64_t param_2, uint64_t param_3)
{
    if (g_RefZoom)
    {
        g_RefZoom->wheelStep = g_RefZoom->wheelStep + (1 * (g_RefZoom->wheelStep < 8));

        float steps = 8.0f;
        float min = 5.0f;
        *g_RefZoom->fov_p = animation::SmoothInverse((float)g_RefZoom->wheelStep, steps, 22.0f, min);
        //std::cout << *g_RefZoom->fov_p << " :: " << g_RefZoom->wheelStep << std::endl;
    }
}

void __fastcall hkDownCallFunc(void* param_1, uint64_t param_2, uint64_t param_3)
{
    if (g_RefZoom)
    {
        g_RefZoom->wheelStep = g_RefZoom->wheelStep - (1 * (g_RefZoom->wheelStep > -8));

        float steps = 8.0f;
        float min = 5.0f;
        *g_RefZoom->fov_p = animation::SmoothInverse((float)g_RefZoom->wheelStep, steps, 22.0f, min);
        //std::cout << *g_RefZoom->fov_p << " :: " << g_RefZoom->wheelStep << std::endl;
    }
}

Zoom::Zoom()
{
    this->m_hotkey = &(gameManager->m_inputListener->m_keyMappings->VK_V);
}

void Zoom::initGameData()
{
    // Set reference for hooks
    g_RefZoom = this;

    fov_p    = (float*)mem::FindDMAAddy(gameManager->m_moduleBase + 0x0369BD40, { 0x20, 0x120, 0x1E8 });
    fovMin_p = (float*)mem::FindDMAAddy(gameManager->m_moduleBase + 0x0369BD40, { 0x20, 0x120, 0x1e0 });
    fovMax_p = (float*)mem::FindDMAAddy(gameManager->m_moduleBase + 0x0369BD40, { 0x20, 0x120, 0x1e4 });

    fovLowLock.initGameData();
    fovMaxLock.initGameData();

    hotbarUpCallFunc.initGameData();
    hotbarDownCallFunc.initGameData();

    void* temp = nullptr;

    if (MH_CreateHook((void*)hotbarUpCallFunc.m_addr, &hkUpCallFunc, (void**)temp) != MH_OK)
        LOG(L"Minhook: create hook hotbarUpCallFunc failed!");

    if (MH_CreateHook((void*)hotbarDownCallFunc.m_addr, &hkDownCallFunc, (void**)temp) != MH_OK)
        LOG(L"Minhook: create hook hotbarDownCallFunc failed!");
}

void Zoom::onStart()
{
    *fovMin_p = 0.0f;
    *fovMax_p = 1000.0f;

    fovLowLock.activate();
    fovMaxLock.activate();
}

void Zoom::onStop()
{
    *fov_p = 76.0f;
    *fovMin_p = 30.0f;
    *fovMax_p = 110.0f;

    /*fovLowLock.deactivate();
    fovMaxLock.deactivate();*/
}

void Zoom::onPress()
{
    animation::LerpFov(fov_p, 76.0f, 19.0f, 0.3f);

    if (MH_EnableHook((void*)hotbarUpCallFunc.m_addr) != MH_OK)
        LOG(L"Minhook: enable hook hotbarUpCallFunc failed!");

    if (MH_EnableHook((void*)hotbarDownCallFunc.m_addr) != MH_OK)
        LOG(L"Minhook: enable hook hotbarDownCallFunc failed!");
}

void Zoom::onRelease()
{
    if (MH_DisableHook((void*)hotbarUpCallFunc.m_addr) != MH_OK)
        LOG(L"Minhook: disable hook hotbarUpCallFunc failed!");

    if (MH_DisableHook((void*)hotbarDownCallFunc.m_addr) != MH_OK)
        LOG(L"Minhook: disable hook hotbarDownCallFunc failed!");

    animation::LerpFov(fov_p, 19.0f, 76.0f, 0.1f);

    wheelStep = 0;
}