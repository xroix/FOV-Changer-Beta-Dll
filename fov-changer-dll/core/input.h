#pragma once

#include "../pch.h"

#include "../utils/Logger.h"
#include "../utils/mem.h"
#include "../SDK/KeyMappings.h"

using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::UI::Core;


namespace Core::Input
{
    typedef void(__fastcall* typeMouseCall)(int64_t, byte, byte, int16_t, int16_t, int16_t, int16_t, byte);
    typedef void(__fastcall* typeKeyboardCall)(int64_t, int32_t);

    class InputListener
    {
    public:
        bool                m_blockGamesInput = false;
        uintptr_t           m_moduleBase;
        byte*               m_pointerCapture_p = nullptr;
        KeyMappings*        m_keyMappings;

        // Hook trampoline calls
        typeMouseCall       m_mouseTrampoline = nullptr;
        typeKeyboardCall    m_keyboardTrampoline = nullptr;


    public:
        InputListener(uintptr_t moduleBase);

        void initGameData();

    };
}


// Controlling function:
// 00eee140
// Called for mouse input based stuff:
// 0262e590
// Called for keyboard related stuff:
// 004e1a40
