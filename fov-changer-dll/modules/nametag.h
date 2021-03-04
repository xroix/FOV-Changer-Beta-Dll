#pragma once

#include "..\pch.h"

#include "..\module.h"


// Hooks
typedef int64_t(__fastcall* typeNametagRenderCall)(int64_t*, int64_t);


// Used to store old player data
class PlayerHolder
{
public:
	// Where the injected pointer to our string is
	void** m_playerNamePointer = nullptr;

	std::string m_oldName;

	std::string m_newName;
	const char* m_newNameHolder;

	PlayerHolder(const std::string& name, void* playerNamePointer);
};

class Nametag : public IModule
{
public:
	std::string m_name = "Nametag";

	std::thread cleanupThread;

	typeNametagRenderCall trampolineCall = nullptr;
	std::unordered_map<void*, std::unique_ptr<PlayerHolder>> players;

	Nametag();

	virtual void initGameData() override;

	virtual void onStart() override;
};
