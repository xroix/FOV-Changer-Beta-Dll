#pragma once

#include "../pch.h"

#include "../client.h"
#include "../core/module.h"
#include "../utils/animation.h"


class Zoom : public Module::IModule
{
public:
	std::string m_name =				 "Zoom";

	// Pointers
	float* fov_p =					     nullptr;
	float* fovMin_p =					 nullptr;
	float* fovMax_p =					 nullptr;

	// Aob
	Signature fovLowLock =		 Signature("F3 0F 5F 35 49 D5 EB 01", 8, "\xF3\x0F\x5F\x35\x49\xD5\xEB\x01");
	Signature fovMaxLock =		 Signature("F3 0F 5D 35 61 D6 EB 01", 8, "\xF3\x0F\x5D\x35\x61\xD6\xEB\x01");

	// Functions that call a function, that changes the hotbar
	// User for getting scrollwheel input while blocking the hotbar 
	Signature hotbarUpCallFunc =	 Signature("40 53 48 83 EC 20 48 8B 59 08 83 BB 7C 0A 00 00 01");
	Signature hotbarDownCallFunc = Signature("40 53 48 83 EC 20 48 8B 19 83 BB 7C 0A 00 00 01");

	// For scroll wheel mechanic
	int wheelStep =						 0;

	Zoom();

	virtual void initGameData() override;

	virtual void onStart() override;
	virtual void onStop() override;
	virtual void onPress() override;
	virtual void onRelease() override;
};
