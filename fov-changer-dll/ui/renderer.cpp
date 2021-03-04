#include "../pch.h"
#include "renderer.h"


// Constructor
UI::Renderer::Renderer()
{
}

void UI::Renderer::Setup(winrt::agile_ref<CoreWindow> window)
{
	//LOG(L"Setting up Renderer");
    OutputDebugString(L"Setting up Renderer");

    // Create resources
    m_deviceResources = std::make_shared<DX::DeviceResources>();

    m_deviceResources->SetWindow(window.get());

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

    OutputDebugString(L"Setting up Renderer FINISHED");
}

void UI::Renderer::Loop()
{
    /*while (true)
    {

    }*/
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


    D2D1_RECT_F rectangle2 = D2D1::RectF(
        100.0f / 2 - 100.0f,
        100.0f / 2 - 100.0f,
        100.0f / 2 + 100.0f,
        100.0f / 2 + 100.0f
    );

    // Draw a filled rectangle.
    m_deviceResources->GetD2DDeviceContext()->FillRectangle(&rectangle2, m_textBrush.get());

    m_deviceResources->GetD2DDeviceContext()->EndDraw();

    m_deviceResources->Present();
}

void UI::RenderingThread(const std::shared_ptr<UI::Renderer> renderer, CoreWindow const& window)
{
    renderer->Setup(window);
}