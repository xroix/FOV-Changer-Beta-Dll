#include "renderer.h"


namespace UI
{
    Renderer::Renderer()
    {
        m_window = CoreApplication::MainView().CoreWindow();
        m_deviceResources = std::make_shared<DX::DeviceResources>();

        // Original UI thread needed
        m_window.get().Dispatcher().RunAsync(CoreDispatcherPriority::Low, [&](auto && ...)
            {
                m_deviceResources->SetWindow(m_window.get());
            }).get();

            // Create brush
            winrt::check_hresult(
                m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(
                    D2D1::ColorF(D2D1::ColorF::White),
                    m_textBrush.put()
                )
            );

            // Create Font
            winrt::check_hresult(
                m_deviceResources->GetDWriteFactory()->CreateTextFormat(
                    L"Open Sans",
                    nullptr,
                    DWRITE_FONT_WEIGHT_REGULAR,
                    DWRITE_FONT_STYLE_NORMAL,
                    DWRITE_FONT_STRETCH_NORMAL,
                    32.0f,
                    L"en-US", // locale
                    m_dwriteTextFormat.put()
                )
            );

            // Create text layout
            winrt::hstring text = L"yes";
            auto size = m_deviceResources->GetLogicalSize();

            winrt::check_hresult(
                m_deviceResources->GetDWriteFactory()->CreateTextLayout(
                    text.c_str(),                              // Text to be displayed
                    text.size(),                               // Length of the text
                    m_dwriteTextFormat.get(),                  // DirectWrite Text Format object
                    size.Width,                                // Width of the Text Layout
                    size.Height,                               // Height of the Text Layout
                    m_dwriteTextLayout.put()
                )
            );
    }

    void Renderer::CreateDeviceDependentResources()
    {
    }

    void Renderer::Render()
    {
        m_deviceResources->GetD2DDeviceContext()->BeginDraw();

        //m_deviceResources->GetD2DDeviceContext()->Clear(D2D1::ColorF(D2D1::ColorF::CornflowerBlue));

        m_deviceResources->GetD2DDeviceContext()->SetTransform(
            m_deviceResources->GetOrientationTransform2D()
        );

        D2D1_POINT_2F origin = D2D1::Point2F(0.0f, 0.0f);

        m_deviceResources->GetD2DDeviceContext()->DrawTextLayout(
            origin,
            m_dwriteTextLayout.get(),
            m_textBrush.get()
        );


        D2D1_RECT_F rectangle1 = D2D1::RectF(
            50.0f,
            50.0f,
            200.0f,
            80.0f
        );
        m_deviceResources->GetD2DDeviceContext()->FillRectangle(&rectangle1, m_textBrush.get());

        D2D1_RECT_F rectangle2 = D2D1::RectF(
            50.0f,
            50.0f,
            200.0f,
            80.0f
        );
        m_deviceResources->GetD2DDeviceContext()->FillRectangle(&rectangle2, m_textBrush.get());

        m_deviceResources->GetD2DDeviceContext()->EndDraw();
    }

    void RenderThread(std::shared_ptr<UI::Main> ui)
    {
        // Initialize renderer here to avoid errors
        ui->m_renderer = std::make_shared<Renderer>();

        ui->Loop();
    }
}