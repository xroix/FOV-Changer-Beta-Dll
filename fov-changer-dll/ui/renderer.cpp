#include "renderer.h"


namespace UI
{
    void RenderThread()
    {
        auto& ui = UI::Main::Get();
        
        // Initialize renderer here to avoid errors
        ui.m_renderer = std::make_shared<Renderer>();
        
        // For later use
        auto renderer = ui.m_renderer;

        // Set events
        ui.InvokeAsUIThread([&]()
            {
                renderer->m_window.get().VisibilityChanged({ &ui, &Main::OnVisibilityChanged });

                renderer->m_window.get().SizeChanged({ &ui, &Main::OnSizeChanged });
            }
        );

        ui.Loop();
    }

    Renderer::Renderer()
    {
        //m_ui = UI::Main::Get();
        m_window = CoreApplication::MainView().CoreWindow();
        m_deviceResources = std::make_shared<DX::DeviceResources>();

        // Original UI thread needed
        UI::Main::Get().InvokeAsUIThread([&]()
            {
                m_deviceResources->SetWindow(m_window.get());
            }
        );

        CreateDeviceIndependentResources();
        CreateDeviceDependentResources();
    }

    void Renderer::CreateDeviceDependentResources()
    {
        auto context = m_deviceResources->GetD2DDeviceContext();

        // Create brushes
        winrt::check_hresult(
            context->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::White),
                m_whiteBrush.put()
            )
        );
        winrt::check_hresult(
            context->CreateSolidColorBrush(
                D2D1::ColorF(34.0f / 255.0f, 47.0f / 255.0f, 62.0f / 255.0f, 0.9f),
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
                L"", // locale
                m_dwriteTextFormat.put()
            )
        );

        // Create text layout
        winrt::hstring text = L"[ Injected ]";
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

        // Create effects
        winrt::com_ptr<ID2D1Effect> shadowEffect;
        winrt::check_hresult(
            context->CreateEffect(CLSID_D2D1Shadow, shadowEffect.put()) // Shadow
        );

        winrt::com_ptr<ID2D1Effect> affineTransformEffect;
        winrt::check_hresult(
            context->CreateEffect(CLSID_D2D12DAffineTransform, affineTransformEffect.put()) // AffineTransform
        );

        winrt::com_ptr<ID2D1Effect> compositeEffect;
        winrt::check_hresult(
            context->CreateEffect(CLSID_D2D1Composite, compositeEffect.put()) // Composite
        );

        shadowEffect->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, 3.0f);
        shadowEffect->SetValue(D2D1_SHADOW_PROP_COLOR, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.5f));
        shadowEffect->SetInput(0, m_bitmapTest.get());
        affineTransformEffect->SetInputEffect(0, shadowEffect.get());

        D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Translation(0, 0);

        affineTransformEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, matrix);

        compositeEffect->SetInputEffect(0, shadowEffect.get());
        compositeEffect->SetInput(1, m_bitmapTest.get());

        m_dropShadowEffect = compositeEffect;

        // Bitmaps
        winrt::check_hresult(
            context->CreateBitmap(context->GetPixelSize(), nullptr, 0,
                D2D1::BitmapProperties1(
                    D2D1_BITMAP_OPTIONS_TARGET,
                    D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                    m_deviceResources->GetDpi(),
                    m_deviceResources->GetDpi()
                ),
                m_bitmap.put()
            )
        );
        

    }

    void Renderer::CreateDeviceIndependentResources() 
    {
        auto wicFactory = m_deviceResources->GetWicImagingFactory();

        try
        {
            // Testing: Bitmap
        // Create a decoder
            winrt::com_ptr<IWICBitmapDecoder> decoder;
            winrt::check_hresult(
                wicFactory->CreateDecoderFromFilename(
                    L"UAP.Assets\\fovchanger\\img.png",
                    nullptr,
                    GENERIC_READ,
                    WICDecodeMetadataCacheOnDemand,
                    decoder.put()
                )
            );

            // Get first frame
            winrt::com_ptr<IWICBitmapFrameDecode> frame;
            winrt::check_hresult(decoder->GetFrame(0, frame.put()));

            // Create a converter
            winrt::com_ptr<IWICFormatConverter> converter;
            winrt::check_hresult(wicFactory->CreateFormatConverter(converter.put()));

            // Convert!
            winrt::check_hresult(
                converter->Initialize(
                    frame.get(),
                    GUID_WICPixelFormat32bppPBGRA,
                    WICBitmapDitherTypeNone,
                    NULL,
                    0.f,
                    WICBitmapPaletteTypeCustom
                )
            );

            m_deviceResources->GetD2DDeviceContext()->CreateBitmapFromWicBitmap(converter.get(), m_bitmapTest.put());
        }
        catch (...)
        {
            m_bitmapTest = nullptr;
        }
    }

    /*
    * Note that currently all sorts of stuffs gets tested out
    * TODO
    */
    void Renderer::Render()
    {
        auto context = m_deviceResources->GetD2DDeviceContext();

        context->BeginDraw();

        //m_deviceResources->GetD2DDeviceContext()->Clear(D2D1::ColorF(D2D1::ColorF::CornflowerBlue));
        m_deviceResources->GetD2DDeviceContext()->Clear();

        context->SetTransform(
            m_deviceResources->GetOrientationTransform2D()
        );

        // [ Injected ]
        /*context->DrawTextLayout(
            D2D1::Point2F(50.0f, 0.0f),
            m_dwriteTextLayout.get(),
            m_whiteBrush.get()
        );*/

        // DEBUG
        /*static BYTE i; i += 1;
        std::wstring test { L"Render Count: " + std::to_wstring(i) + L"\nInputEnabled: " + std::to_wstring(m_test)};
        context->DrawTextW(test.c_str(), test.size(), m_dwriteTextFormat.get(), D2D1::RectF(8.0f, 250.0f, 1000.0f, 1000.0f), m_whiteBrush.get());*/

        if (m_bitmapTest && true)
        {
            D2D1_SIZE_F size = m_bitmapTest->GetSize();
            D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(this->m_deviceResources->GetLogicalSize().Width / 2 - size.width / 1 / 2, 10.0f);

            //m_dropShadowEffect->SetInput(0, m_bitmapTest.get());
            context->DrawImage(
                m_dropShadowEffect.get(),
                upperLeftCorner
            );

            /*context->DrawBitmap(
                m_bitmapTest.get(), 
                D2D1::RectF(
                    upperLeftCorner.x, 
                    upperLeftCorner.y, 
                    upperLeftCorner.x + size.width / 1, 
                    upperLeftCorner.y + size.height / 1),
                1.0f,
                D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
            );*/
        }

        // Testing out a debug output
        if (m_bitmap && false)
        {
            winrt::check_hresult(
                context->EndDraw()
            );
            winrt::com_ptr<ID2D1Image> old;
            context->GetTarget(old.put());
            context->SetTarget(m_bitmap.get());
            context->BeginDraw();
            context->Clear();

            const FLOAT width = 700.0f;
            const FLOAT height = 200.0f;
            const D2D1_POINT_2F upperRightCorner = D2D1::Point2F(width + 10.0f, 10.0f); // this->m_bitmap->GetSize().width + 10.0f
            const D2D1_ROUNDED_RECT container = D2D1::RoundedRect(D2D1::RectF(
                upperRightCorner.x - width,
                upperRightCorner.y,
                upperRightCorner.x,
                upperRightCorner.y + height
            ), 10.0f, 10.0f);
            context->FillRoundedRectangle(&container, m_blackBrush.get());

            static BYTE i; i += 1;
            std::wstring test{ L"i=" + std::to_wstring(i) + L"\nsooooooooooooooooooome loooooooooooooooooong teeeeeeeeeeext"};
            const FLOAT margin = 20.0f;
            const D2D1_RECT_F textBox = D2D1::RectF(upperRightCorner.x - width + margin, upperRightCorner.y + margin, upperRightCorner.x - margin, upperRightCorner.y + height - margin);
            context->DrawTextW(
                test.c_str(), test.size(), m_dwriteTextFormat.get(), 
                textBox,
                m_whiteBrush.get()
            );

            winrt::check_hresult(
                context->EndDraw()
            );
            context->SetTarget(old.get());

            context->BeginDraw();
            context->DrawBitmap(m_bitmap.get()); // TESTING, switch back to dropshadow!!!!!!!!!!!
        }

        D2D1_POINT_2F origin = D2D1::Point2F(50.0f, this->m_deviceResources->GetLogicalSize().Height - 250.0f);
        const FLOAT tile_x_len = 60.0f;
        const FLOAT tile_y_len = 60.0f;
        const FLOAT space_y_len = tile_y_len / 1.5f;
        const FLOAT margin = 15.0f;  // Note: only inner
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

        // Forwards
        //if (gameManager->m_inputListener->m_keyMappings->VK_W)
        //    context->FillRectangle(&forward, m_whiteBrush.get());

        //else
        //    context->FillRectangle(&forward, m_blackBrush.get());

        //// Backwards
        //if (gameManager->m_inputListener->m_keyMappings->VK_S)
        //    context->FillRectangle(&backwards, m_whiteBrush.get());

        //else
        //    context->FillRectangle(&backwards, m_blackBrush.get());

        //// Left
        //if (gameManager->m_inputListener->m_keyMappings->VK_A)
        //    context->FillRectangle(&left, m_whiteBrush.get());

        //else
        //    context->FillRectangle(&left, m_blackBrush.get());

        //// Right
        //if (gameManager->m_inputListener->m_keyMappings->VK_D)
        //    context->FillRectangle(&right, m_whiteBrush.get());

        //else
        //    context->FillRectangle(&right, m_blackBrush.get());

        //// Space
        //if (gameManager->m_inputListener->m_keyMappings->SPACE)
        //    context->FillRectangle(&space, m_whiteBrush.get());

        //else
        //    context->FillRectangle(&space, m_blackBrush.get());

        /*context->FillRectangle(&backwards, m_blackBrush.get());
        context->FillRectangle(&left, m_blackBrush.get());
        context->FillRectangle(&right, m_blackBrush.get());
        context->FillRectangle(&space, m_blackBrush.get());*/

        /*context->DrawRectangle(&forward, m_blackBrush.get(), outline, NULL);
        context->DrawRectangle(&backwards, m_blackBrush.get(), outline, NULL);
        context->DrawRectangle(&left, m_blackBrush.get(), outline, NULL);
        context->DrawRectangle(&right, m_blackBrush.get(), outline, NULL);
        context->DrawRectangle(&space, m_blackBrush.get(), outline, NULL);*/

        winrt::check_hresult(
            context->EndDraw()
        );
    }
}