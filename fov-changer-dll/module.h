#pragma once

#include "pch.h"

#include "game.h"
#include "utils\mem.h"
#include "utils\proc.h"
// #include "modules/zoom.h" Dont include yet because Module (here created) is needed there


class Signature
{
public:
	std::string m_signature;
	const char* m_aob1;  // Must be char* because std::string does not work, idk why (in some cases)
	const char* m_aob2;  // Must be char* because std::string does not work, idk why (in some cases)
	int m_aobLength;

	uintptr_t m_addr;

	// Just to get addr, use mem::GetAddressFromSignature
	Signature(const std::string& signature);
	// Will have toggeable nopping and setting aob
	Signature(const std::string& signature, int aobLength, const char* aob);
	// Will have two toggeable
	Signature(const std::string& signature, int aobLength, const char* aob1, const char* aob2);

	// Deactivate if needed
	~Signature();

	void initGameData();

	void activate();
	void deactivate();

private:
	unsigned int _Mode;
};




// Get inherited in ./modules
class IModule
{
public:
	std::string m_name = "Not named module";

	int* m_hotkey = nullptr;
	bool m_pressed = false;

	IModule() {}
	~IModule() {}

	// Called opon startup to get pointer etc.
	virtual void initGameData() {}

	// Called after initGameData
	virtual void onStart() {}
	// Called on deconstruction of the module manager
	virtual void onStop() {}
	// Called on key press
	virtual void onPress() {}
	// Called on key release
	virtual void onRelease() {}

private:
};

// Handles Module instances
class ModuleManager
{
public:
	std::vector<std::shared_ptr<IModule>> m_modules;

	ModuleManager();
	~ModuleManager();

	void initGameData();
	void loopHotkeys();
};