#include "game.h"


GameManager::GameManager()
{
    m_moduleBase = (uintptr_t)GetModuleHandle(L"Minecraft.Windows.exe");
    m_inputListener = std::make_shared<Core::Input::InputListener>(m_moduleBase);
}