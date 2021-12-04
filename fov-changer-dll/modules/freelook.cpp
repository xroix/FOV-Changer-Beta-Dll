#include "..\pch.h"
#include "freelook.h"


Freelook::Freelook()
{
    this->m_hotkey = &(Client::GetInputListener().m_keyMappings->CTRL);
}

void Freelook::initGameData()
{
    currentPerspective_p = (int*)mem::FindDMAAddy(Client::Get().m_moduleBase + 0x037CB460, { 0x18, 0x30, 0x18 });
    lastPerspective = *currentPerspective_p;

    freelookHead.initGameData();
    freelookBody.initGameData();

    //std::cout << std::hex << freelookHead.m_Addr << " <> " << std::hex << freelookBody.m_Addr  << std::endl;
}

void Freelook::onPress()
{
    lastPerspective = *currentPerspective_p;

    // Set to main to make use of the bug
    // that when you set back to 0, the orientation is restored

    // Set it and make possible, that it goes from 2 perspective
    *currentPerspective_p = 1 + 1 * (lastPerspective == 2);

    freelookHead.Activate();
    freelookBody.Activate();
}

void Freelook::onRelease()
{
    if (lastPerspective != 0)
    {
        *currentPerspective_p = 0;
        Sleep(6);
    }

    *currentPerspective_p = lastPerspective;

    freelookHead.Deactivate();
    freelookBody.Deactivate();
}