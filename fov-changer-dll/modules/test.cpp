#include "..\pch.h"
#include "test.h"

#include "..\utils\Logger.h"


// Used at hooks to get Zoom class
static Test* g_refTest = nullptr;


bool __fastcall hkGetResource(void* _this, void* param_1, const char** param_2, int param_3)
{
    try
    {
        if (g_refTest)
        {
            std::wostringstream formated_msg;
            formated_msg << L"hkGetResource: 0: " << _this << L" 1: " << param_1 << L" 2: " << *param_2 << L" 3: " << param_3 << L"\n";
            OutputDebugString(formated_msg.str().c_str());

            if (g_refTest->m_getResourceTrampoline)
                return g_refTest->m_getResourceTrampoline(_this, param_1, param_2, param_3);
        }
        return false;
    }
    catch (...)
    {
    }
}

Test::Test()
{
    this->m_hotkey = &(Client::GetInputListener().m_keyMappings->VK_V);
}

void Test::initGameData()
{
    // Set reference for hooks
    g_refTest = this;

    void* temp = nullptr;

    if (MH_CreateHook((void*)(Client::Get().m_moduleBase + 0x1405240), &hkGetResource, reinterpret_cast<void**>(&m_getResourceTrampoline)) != MH_OK)
        LOG(L"Minhook: create hook hkGetResource failed!");

    /*if (MH_EnableHook((void*)(gameManager->m_moduleBase + 0x1405240)) != MH_OK)
        LOG(L"Minhook: enable hook hkGetResource failed!");*/
}

void Test::onStart()
{

}

void Test::onStop()
{

}
