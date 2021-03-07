#pragma once

#include "../pch.h"

#include "../game.h"
#include "../core/module.h"


class Freelook : public Core::Module::IModule
{
public:
	std::string		m_name = "Freelook";

	// Pointer
	int*			currentPerspective_p = nullptr;
	int				lastPerspective = 0;

	// Aob
	Core::Signature	freelookHead = Core::Signature("F3 0F 11 83 90 06 00 00 48 8B 93 00 03 00 00", 8, "\xF3\x0F\x11\x83\x28\x06\x00\x00");
	Core::Signature	freelookBody = Core::Signature("FF 90 70 01 00 00 84 C0 0F 85 DC 00 00 00", 6, "\xFF\x90\x70\x01\x00\x00");

	Freelook();

	virtual void initGameData() override;

	virtual void onPress() override;
	virtual void onRelease() override;
};
