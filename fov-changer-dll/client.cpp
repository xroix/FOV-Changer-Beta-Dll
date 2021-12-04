#include "client.h"


Client::Client()
{
    m_moduleBase = (uintptr_t)GetModuleHandle(L"Minecraft.Windows.exe");
}