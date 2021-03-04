#include "..\pch.h"
#include "nametag.h"


extern const std::shared_ptr<GameManager> gameManager;


// Used at hooks to get Nametag class
Nametag* g_RefNameTag = nullptr;

std::mutex mtx;

int64_t __fastcall hkNametagRenderCall(int64_t* param_1, int64_t param_2)
{
    // Pointer to the address of a the gamertag string
    void* str_p = param_1 + 0xa4;

    mtx.lock();  // Lock

    // Check if name is already captured
    if ((g_RefNameTag->players.find(str_p) != g_RefNameTag->players.end())) // && *(void**)str_p == (void*)g_RefNameTag->players[str_p]->m_newNameHolder)
    {    
    }

    // Capture it!!!
    else
    {
        const char* gamertag = nullptr;

        // Address string_p is a pointer
        if (0xf < *(size_t*)(param_1 + 0xa4 + 3))
        {
            if ((const char*)*(param_1 + 0xa4))
            {
                gamertag = (const char*)*(param_1 + 0xa4);
            }

        }

        // Address string_p is directly the string
        else
        {
            if ((const char*)(param_1 + 0xa4))
            {
                gamertag = (const char*)(param_1 + 0xa4);
            }
        }

        if (gamertag && gamertag != "\0" && gamertag != " " && gamertag != "" && gamertag != nullptr)
        {
            std::cout << "Adding: " << gamertag << " Addr: " << str_p << std::endl;

            // Add it to the map
            g_RefNameTag->players[str_p] = std::make_unique<PlayerHolder>(gamertag, str_p);
            
            // Save old for later
            char* old = (char*)str_p;

            // Copy pointer into player obj
            memcpy(str_p, &g_RefNameTag->players[str_p]->m_newNameHolder, sizeof(void*));

            // Fix size
            uintptr_t length = g_RefNameTag->players[str_p]->m_newName.length();
            *(uintptr_t*)(param_1 + 0xa4 + 2) = length;  // Rendering length
            *(uintptr_t*)(param_1 + 0xa4 + 3) = length < 17 ? 17 : length;  // If it is a pointer
        
        } else
        {
            std::cout << "OH NO" << std::endl;
            mtx.unlock();
            return g_RefNameTag->trampolineCall(param_1, param_2);
        }
    }

    mtx.unlock();  // Lock

    char* old = (char*)str_p;  // Save old content
    std::memcpy(str_p, &g_RefNameTag->players[str_p]->m_newNameHolder, sizeof(void*));  // Write new

    int64_t returnVal = g_RefNameTag->trampolineCall(param_1, param_2);  // Render with new

    std::memcpy(str_p, old, sizeof(void*));  // Write back old

    return returnVal;
}

void CleanupThread(Nametag* nametagModule)
{
    std::cout << "CleanupThread started" << std::endl;

    for (;;)
    {   
        mtx.lock();  // Lock

        std::cout << "Doing clean up, size: " << nametagModule->players.size() << std::endl;

        for (auto it = nametagModule->players.begin(); it != nametagModule->players.end();)
        {
            if (!it->second->m_newNameHolder || *it->second->m_playerNamePointer != (void*)it->second->m_newNameHolder)
            {
                std::cout << "Deleting player: " << it->second->m_oldName << std::endl;
                it = nametagModule->players.erase(it);
            } else
                it++;
        }

        mtx.unlock();  // Locks

        Sleep(4000);
    }
}


PlayerHolder::PlayerHolder(const std::string& name, void* playerNamePointer)
{
    this->m_playerNamePointer = (void**)playerNamePointer;

    this->m_oldName = name;
    this->m_newName = name + " \xEE\x84\x85";
    this->m_newNameHolder = this->m_newName.c_str();
}

Nametag::Nametag()
{
    // Used to clear left players from players
    cleanupThread = std::thread(CleanupThread, this);
}

void Nametag::initGameData()
{
    // For hooks to get this class instance
    g_RefNameTag = this;

    // Create hooks
    if (MH_CreateHook((void*)(gameManager->m_moduleBase + 0x130c730), &hkNametagRenderCall, reinterpret_cast<LPVOID*>(&trampolineCall)) != MH_OK)
        std::cout << "Minhook: create hook hkNametagRenderCall failed!" << std::endl;
}

void Nametag::onStart()
{
    // Enable needed hooks
    /*if (MH_EnableHook((void*)(gameManager->m_moduleBase + 0x130c730)) != MH_OK)
        std::cout << "Minhook: enable hook hkNametagRenderCall failed!" << std::endl;*/
}

