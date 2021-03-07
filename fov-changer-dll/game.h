#pragma once

#include "pch.h"
#include "core/input.h"
#include "core/module.h"
#include "SDK/KeyMappings.h"
#include "utils/mem.h"
#include "utils/proc.h"
#include "utils/Logger.h"


class GameManager
{
public:
    GameManager();

public:
    uintptr_t                                      m_moduleBase;
    std::shared_ptr<Core::Input::InputListener>    m_inputListener;
};