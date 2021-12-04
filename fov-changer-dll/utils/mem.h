#pragma once

#include "..\pch.h"
#include <windows.h>

//https://guidedhacking.com/threads/how-to-hack-any-game-first-internal-hack-dll-tutorial.12142/

namespace mem
{
	// ASM Injection
	void Patch(BYTE* dst, BYTE* src, unsigned int size);
	void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess);
	void Nop(BYTE* dst, unsigned int size);
	void NopEx(BYTE* dst, unsigned int size, HANDLE hProcess);
	bool ValidMemory(uintptr_t addr);
	// Multilevel pointers
	uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);
	// Signature-Pattern scanning
	uintptr_t GetAddressFromSignature(const char* szModule, const char* szSignature);
	const char* AobToSignature(std::vector<unsigned int> aob);
}