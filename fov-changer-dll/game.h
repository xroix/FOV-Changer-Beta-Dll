#pragma once

#include "pch.h"
#include "utils\mem.h"
#include "utils\proc.h"
#include "SDK/KeyListener.h"


class GameManager
{
public:
    uintptr_t m_moduleBase;
    KeyListener* c_KeyListener;

    GameManager();
};