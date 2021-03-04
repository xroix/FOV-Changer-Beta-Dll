#include "pch.h"
#include "game.h"

GameManager::GameManager()
{
    this->m_moduleBase = (uintptr_t)GetModuleHandle(L"Minecraft.Windows.exe");
    this->c_KeyListener = (KeyListener*)(this->m_moduleBase + 0x036529B0);
}