#pragma once

#include "pch.h"
#include "utils\mem.h"
#include "utils\proc.h"
#include "utils\Logger.h"
#include "SDK/KeyListener.h"


class GameManager;
class InputListener;

typedef void(__fastcall* typeMouseCall)(int64_t, byte, byte, int16_t, int16_t, int16_t, int16_t, byte);
typedef void(__fastcall* typeKeyboardCall)(int64_t, int32_t);


class GameManager
{
public:
    uintptr_t                            m_moduleBase;
    std::shared_ptr<InputListener>       m_inputListener;


    GameManager();
};

class InputListener
{
public:
    GameManager*        m_gameManager = nullptr;
    bool                m_blockGamesInput = false;
    uintptr_t           m_moduleBase;
    byte*               m_pointerCapture_p = nullptr;
    KeyMappings*        m_keyMappings;

    // Hook trampoline calls
    typeMouseCall       m_mouseTrampoline = nullptr;
    typeKeyboardCall    m_keyboardTrampoline = nullptr;


public:
    InputListener(GameManager* gameManager);

    void initGameData();

};

// Controlling function:
// 00eee140
// Called for mouse input based stuff:
// 0262e590
// Called for keyboard related stuff:
// 004e1a40
