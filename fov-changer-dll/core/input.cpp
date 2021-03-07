#include "input.h"


// For hooks
static Core::Input::InputListener* g_refInputListener = nullptr;


/*
* somePointer:      idk
* mode:             0: moving, 1: left-click, 2: right-click, 3: middle-click, 4: scrolling
* pressed:          1 if press, 0 if release
* mouseX:           x coords
* mouseY:           y coords
* mouseXvelocity:   x velocity if pointer is captured ( CoreWindow::SetPointerCapture(); )
* mouseYvelocity:   y velocity if pointer is captured ( CoreWindow::SetPointerCapture(); )
* idk:              idk
*/
void __fastcall hkMouse(int64_t somePointer, byte mode, byte pressed, int16_t mouseX, int16_t mouseY, int16_t mouseXvelocity, int16_t mouseYvelocity, byte idk)
{
    if (g_refInputListener)
    {
        std::wostringstream formated_msg;
        formated_msg << L"hkMouse: 1: " << somePointer << L" 2: " << mode << L" 3: " << pressed << L" 4: " << mouseX << L" 5: " << mouseY << L" 6: " << mouseXvelocity << L" 7: " << mouseYvelocity << L" 8: " << idk << L"\n";
        OutputDebugString(formated_msg.str().c_str());

        // Dont call trampoline if we block it
        if (!g_refInputListener->m_blockGamesInput)
            g_refInputListener->m_mouseTrampoline(somePointer, mode, pressed, mouseX, mouseY, mouseXvelocity, mouseYvelocity, idk);
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
            if (pressed)
            {
                g_refInputListener->m_blockGamesInput = true;
                CoreApplication::MainView().CoreWindow().ReleasePointerCapture();
                //CoreApplication::MainView().CoreWindow().PointerCursor(CoreCursor(CoreCursorType::Arrow, 0));
                //*g_refInputListener->m_pointerCapture_p = 0;
            } else
            {
                g_refInputListener->m_blockGamesInput = false;
                *g_refInputListener->m_pointerCapture_p = 1; // Sadly, this is needed
                CoreApplication::MainView().CoreWindow().SetPointerCapture();
                CoreApplication::MainView().CoreWindow().PointerCursor(nullptr);

            }
        }

        // Dont call trampoline if we block it
        if (!g_refInputListener->m_blockGamesInput)
            g_refInputListener->m_keyboardTrampoline(keyCode, pressed);
    }
}


namespace Core::Input
{
    InputListener::InputListener(uintptr_t moduleBase)
        : m_moduleBase(moduleBase),
        m_keyMappings((KeyMappings*)(moduleBase + 0x036529B0))
    {
    }

    void InputListener::initGameData()
    {
        g_refInputListener = this;

        m_pointerCapture_p = (byte*)mem::FindDMAAddy(m_moduleBase + 0x03A27CF8, { 0, 0x18, 0, 0x110, 0x2E0, 0x8, 0x48 + 2 });

        // Register hooks
        if (MH_CreateHook((void*)(m_moduleBase + 0x262e590), &hkMouse, reinterpret_cast<LPVOID*>(&m_mouseTrampoline)) != MH_OK)
            LOG(L"Minhook: create hook InputListerner: hkMouse failed!");

        if (MH_CreateHook((void*)(m_moduleBase + 0x4e1a40), &hkKeyboard, reinterpret_cast<LPVOID*>(&m_keyboardTrampoline)) != MH_OK)
            LOG(L"Minhook: create hook InputListerner: hkKeyboard failed!");

        // Enable Hooks
        if (MH_EnableHook((void*)(m_moduleBase + 0x262e590)) != MH_OK)
            LOG(L"Minhook: enable hook InputListerner: hkMouse failed!");

        if (MH_EnableHook((void*)(m_moduleBase + 0x4e1a40)) != MH_OK)
            LOG(L"Minhook: enable hook InputListerner: hkKeyboard failed!");
    }

}