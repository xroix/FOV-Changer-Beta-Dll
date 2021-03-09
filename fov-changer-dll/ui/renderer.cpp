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
            }
        ).get();

        // Create brushes
        winrt::check_hresult(
            m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::White),
                m_whiteBrush.put()
            )
        );
        winrt::check_hresult(
            m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::Black),
                m_blackBrush.put()
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
        winrt::hstring text = L"Injected";
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

    void Renderer::CreateDeviceIndependentResources() {
    }

    void Renderer::Render()
    {
        auto context = m_deviceResources->GetD2DDeviceContext();

        context->BeginDraw();

        //m_deviceResources->GetD2DDeviceContext()->Clear(D2D1::ColorF(D2D1::ColorF::CornflowerBlue));

        context->SetTransform(
            m_deviceResources->GetOrientationTransform2D()
        );

        context->DrawTextLayout(
            D2D1::Point2F(0.0f, 0.0f),
            m_dwriteTextLayout.get(),
            m_whiteBrush.get()
        );

        D2D1_POINT_2F origin = D2D1::Point2F(50.0f, this->m_deviceResources->GetLogicalSize().Height - 250.0f);
        const FLOAT tile_x_len = 60.0f;
        const FLOAT tile_y_len = 60.0f;
        const FLOAT space_y_len = tile_y_len / 1.5f;
        const FLOAT margin = 20.0f;  // Note: only inner
        const FLOAT outline = 2.0f;

        /* Keystrokes
            []
          [][][]
          [....]

        */
        D2D1_RECT_F forward = D2D1::RectF(
            origin.x + tile_x_len + margin,
            origin.y,
            origin.x + tile_x_len + margin + tile_x_len,
            origin.y + tile_y_len
        );
        D2D1_RECT_F backwards = D2D1::RectF(
            origin.x + tile_x_len + margin,
            origin.y + tile_y_len + margin,
            origin.x + tile_x_len + margin + tile_x_len,
            origin.y + tile_y_len + margin + tile_y_len
        );
        D2D1_RECT_F left = D2D1::RectF(
            origin.x,
            origin.y + tile_y_len + margin,
            origin.x + tile_x_len,
            origin.y + tile_y_len + margin + tile_y_len
        );
        D2D1_RECT_F right = D2D1::RectF(
            origin.x + tile_x_len + margin + tile_x_len + margin,
            origin.y + tile_y_len + margin,
            origin.x + tile_x_len + margin + tile_x_len + margin + tile_x_len,
            origin.y + tile_y_len + margin + tile_y_len
        );
        D2D1_RECT_F space = D2D1::RectF(
            origin.x,
            origin.y + tile_y_len + margin + tile_y_len + margin,
            origin.x + tile_x_len + margin + tile_x_len + margin + tile_x_len,
            origin.y + tile_y_len + margin + tile_y_len + margin + space_y_len
        );

        context->FillRectangle(&forward, m_whiteBrush.get());
        context->FillRectangle(&backwards, m_whiteBrush.get());
        context->FillRectangle(&left, m_whiteBrush.get());
        context->FillRectangle(&right, m_whiteBrush.get());
        context->FillRectangle(&space, m_whiteBrush.get());

        context->DrawRectangle(&forward, m_blackBrush.get(), outline, NULL);
        context->DrawRectangle(&backwards, m_blackBrush.get(), outline, NULL);
        context->DrawRectangle(&left, m_blackBrush.get(), outline, NULL);
        context->DrawRectangle(&right, m_blackBrush.get(), outline, NULL);
        context->DrawRectangle(&space, m_blackBrush.get(), outline, NULL);

        context->EndDraw();
    }

    void RenderThread(std::shared_ptr<UI::Main> ui)
    {
        // Initialize renderer here to avoid errors
        ui->m_renderer = std::make_shared<Renderer>();

        ui->Loop();
    }
}