#pragma once

#include "pch.h"
#include "core/input.h"
#include "core/module.h"
#include "ui/main.h"
#include "utils/proc.h"
#include "utils/Logger.h"


// Singleton
class Client
{
public:
    Client(const Client& other) = delete;
    static Client& Get() { static Client instance; return instance; };

    static UI::Main& GetUI() { return UI::Main::Get(); }
    static Module::ModuleManager& GetModuleManager() { return Module::ModuleManager::Get(); }
    static Input::InputListener& GetInputListener() { return Input::InputListener::Get(); } 
    
public:
    uintptr_t   m_moduleBase;

private:
    Client();
};