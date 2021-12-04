#include "input.h"
#include "../client.h"


// For hooks
static Input::InputListener* g_refInputListener = nullptr;
static bool lastP = false;


/*
* mouseStruct:      A struct containing somewhat parsed info of this func, see ghidra for more
* mode:             0: moving, 1: left-click, 2: right-click, 3: middle-click, 4: scrolling
* pressed:          1 if press, 0 if release
* mouseX:           x coords
* mouseY:           y coords
* mouseXvelocity:   x velocity if pointer is captured ( CoreWindow::SetPointerCapture(); )
* mouseYvelocity:   y velocity if pointer is captured ( CoreWindow::SetPointerCapture(); )
* idk:              idk
*/
void __fastcall hkMouse(int64_t mouseStruct_p, byte mode, byte pressed, int16_t mouseX, int16_t mouseY, int16_t mouseXvelocity, int16_t mouseYvelocity, byte idk)
{
    if (g_refInputListener)
    {
        /*std::wostringstream formated_msg;
        formated_msg << L"hkMouse: 1: " << somePointer << L" 2: " << mode << L" 3: " << pressed << L" 4: " << mouseX << L" 5: " << mouseY << L" 6: " << mouseXvelocity << L" 7: " << mouseYvelocity << L" 8: " << idk << L"\n";
        OutputDebugString(formated_msg.str().c_str());*/

        // Dont call trampoline if we block it
        if (!g_refInputListener->m_blockGamesInput)
            g_refInputListener->m_mouseTrampoline(mouseStruct_p, mode, pressed, mouseX, mouseY, mouseXvelocity, mouseYvelocity, idk);
    }
}

/*
* keyCode:  Ascii value of pressed button
* pressed:  1 if press, 0 if release
*/
void __fastcall hkKeyboard(int64_t keyCode, int32_t pressed)
{ 
    if (g_refInputListener)
    {
        std::wostringstream formated_msg;
        formated_msg << L"hkKeyboard: " << keyCode << L" & " << pressed << L"\n";
        OutputDebugString(formated_msg.str().c_str());

        // testing with H key
        if (keyCode == 72)
        {
            auto coreWindow = CoreApplication::MainView().CoreWindow();

            auto& ui = Client::GetUI();
            if (ui.m_renderer)
                ui.m_renderer->m_test = !pressed;

            if (pressed)
            {
                // Set true, if there was already one
                lastP = coreWindow.PointerCursor() != nullptr;

                //g_refInputListener->m_blockGamesInput = true;
                coreWindow.PointerCursor(CoreCursor(CoreCursorType::Arrow, 0));
                coreWindow.ReleasePointerCapture();
                
            } else
            {
                g_refInputListener->m_blockGamesInput = false;
                coreWindow.SetPointerCapture();
                if (!lastP)
                    coreWindow.PointerCursor(nullptr);

            }
        }

 
        // Dont call trampoline if we block it
        if (!g_refInputListener->m_blockGamesInput)
            g_refInputListener->m_keyboardTrampoline(keyCode, pressed);
    }
}


namespace Input
{
    InputListener::InputListener()
        : m_moduleBase(Client::Get().m_moduleBase),
          m_keyMappings((KeyMappings*)(m_moduleBase + 0x402ea20))
    {
    }

    void InputListener::initGameData()
    {
        g_refInputListener = this;

        m_pointerCapture_p = (byte*)mem::FindDMAAddy(m_moduleBase + 0x402D0D0, { 0x2F8 });

        uintptr_t hkMouseAddr = mem::GetAddressFromSignature("Minecraft.Windows.exe", "48 ? ? 48 ? ? ? 48 ? ? ? 48 ? ? ? ? 41 ? 41 ? 41 ? 41 ? 48 ? ? ? 44 ? ? ? ? ? ? ? ? 48");
        uintptr_t hkKeyboardAddr = mem::GetAddressFromSignature("Minecraft.Windows.exe", "48 ? ? ? ? 57 48 ? ? ? 8b ? ? ? ? ? 8B ? 89"); 

        // Register hooks
        if (MH_CreateHook((void*)hkMouseAddr, &hkMouse, reinterpret_cast<LPVOID*>(&m_mouseTrampoline)) != MH_OK)
            LOG(L"Minhook: create hook InputListerner: hkMouse failed!");

        if (MH_CreateHook((void*)hkKeyboardAddr, &hkKeyboard, reinterpret_cast<LPVOID*>(&m_keyboardTrampoline)) != MH_OK)
            LOG(L"Minhook: create hook InputListerner: hkKeyboard failed!");

        // Enable Hooks
        if (MH_EnableHook((void*)hkMouseAddr) != MH_OK)
            LOG(L"Minhook: enable hook InputListerner: hkMouse failed!");

        if (MH_EnableHook((void*)hkKeyboardAddr) != MH_OK)
            LOG(L"Minhook: enable hook InputListerner: hkKeyboard failed!");
    }

}