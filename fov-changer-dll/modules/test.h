#pragma once

#include "../pch.h"

#include "../client.h"
#include "../core/module.h"


typedef bool(__thiscall* typeGetResource)(void*, void*, const char**, int);

class Test : public Module::IModule
{
public:
	std::string m_name = "Testing";

	typeGetResource m_getResourceTrampoline = nullptr;

public:
	Test();

	virtual void initGameData() override;

	virtual void onStart() override;
	virtual void onStop() override;
};
