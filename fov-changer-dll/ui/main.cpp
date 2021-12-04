#include "main.h"


namespace UI
{
    Main::Main()
        : m_window(CoreApplication::MainView().CoreWindow())
    {
        m_dispatcher = m_window.get().Dispatcher();
        // Events are set in render thread
    }

    void Main::StartRenderThread()
    {
        m_renderThread = std::thread(&RenderThread);
    }

    // No need for this func
    void Main::SetWindow(winrt::agile_ref<CoreWindow> window)
    {
        m_window = window;
    }

    // Gets executed in UI::RenderThread, hence, in an **own thread**
    void Main::Loop()
    {
        while (true)
        {
            if ((m_visible && m_renderer->m_renderNeeded) || true)
            {
                m_renderer->Render();
                m_renderer->m_deviceResources->Present();
                
                m_renderer->m_renderNeeded = false;
            }        
        }
    }

    void Main::InvokeAsUIThread(DispatchedHandler const& agileCallback)
    {
        m_dispatcher.RunAsync(CoreDispatcherPriority::Normal, agileCallback).get();
    }

    void Main::InvokeAsUIThreadAsync(DispatchedHandler const& agileCallback)
    {
        m_dispatcher.RunAsync(CoreDispatcherPriority::Normal, agileCallback);
    }

    void Main::OnVisibilityChanged(IInspectable const&, VisibilityChangedEventArgs const& args)
    {
        // Set visible cmds
        m_visible = args.Visible();
        if (m_renderer)
            m_renderer->m_visible = m_visible;

        OutputDebugString(L"Visibility changed. \n");
    }

    void Main::OnSizeChanged(IInspectable const&, WindowSizeChangedEventArgs const& args)
    {
        // Update size
        if (m_renderer)
        {
            Size size = args.Size();

            // We dont want to block!
            InvokeAsUIThreadAsync([size, this]() {
                this->m_renderer->m_deviceResources->SetLogicalSize(size);
                this->m_renderer->m_renderNeeded = true;
            });
        }
        //OutputDebugString(L"\n\n---------------- OnSizeChanged - Hey Guys! ----------------\n\n");
    }
}