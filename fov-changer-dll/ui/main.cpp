#include "main.h"


namespace UI
{
    Main::Main()
        : m_window(CoreApplication::MainView().CoreWindow())
    {
        //m_window.get().VisibilityChanged({ this, &Main::OnVisibilityChanged });
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
        while (true)
        {
            if (m_visible)
            {
                m_renderer->Render();
                m_renderer->m_deviceResources->Present();
            }        
        }
    }

    void Main::OnVisibilityChanged(CoreWindow const&, VisibilityChangedEventArgs const& args)
    {
        m_visible = args.Visible();
        if (m_renderer)
            m_renderer->m_visible = args.Visible();
    }
}