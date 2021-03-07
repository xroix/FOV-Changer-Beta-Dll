#include "main.h"


namespace UI
{
    Main::Main()
        : m_window(CoreApplication::MainView().CoreWindow())
    {
    }

    void Main::StartRenderThread(std::shared_ptr<Main> ui)
    {
        m_renderThread = std::thread(&RenderThread, ui);
    }

    // No need for this func
    void Main::SetWindow(winrt::agile_ref<CoreWindow> window)
    {
        m_window = window;
    }

    void Main::Loop()
    {
        m_renderer->Render();
        m_renderer->m_deviceResources->Present();
    }
}