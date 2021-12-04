#pragma once

#include "../pch.h"

#include "../utils/Logger.h"
#include "../utils/mem.h"
#include "../SDK/KeyMappings.h"

using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::UI::Core;

// Testing
#include <winrt/Windows.UI.Popups.h>
using namespace winrt::Windows::UI::Popups;


namespace Input
{
    typedef void(__fastcall* typeMouseCall)(int64_t, byte, byte, int16_t, int16_t, int16_t, int16_t, byte);
    typedef void(__fastcall* typeKeyboardCall)(int64_t, int32_t);

    class InputListener
    {
    public:
        bool                m_blockGamesInput{ false };
        uintptr_t           m_moduleBase;
        byte*               m_pointerCapture_p;
        KeyMappings*        m_keyMappings;

        // Hook trampoline calls
        typeMouseCall       m_mouseTrampoline{ nullptr };
        typeKeyboardCall    m_keyboardTrampoline{ nullptr };


    public:
        InputListener(const InputListener& other) = delete;
        static InputListener& Get() { static InputListener instance; return instance; }

        void initGameData();

    private:
        InputListener();

    };
}

// Outdated!
// Controlling function:
// Minecraft.Windows.exe+E49EE0 
// Called for mouse input based stuff:
// Minecraft.Windows.exe+2364350
// Called for keyboard related stuff:
// Minecraft.Windows.exe+529F90 

