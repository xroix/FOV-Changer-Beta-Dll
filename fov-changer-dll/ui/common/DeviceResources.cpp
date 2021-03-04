﻿//********************************************************* 
// 
// Copyright (c) Microsoft. All rights reserved. 
// This code is licensed under the MIT License (MIT). 
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY 
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR 
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT. 
// 
//*********************************************************

#include "../../pch.h"
#include "DeviceResources.h"
#include "DirectXHelper.h"

using namespace D2D1;
using namespace DirectX;
using namespace winrt::Windows::Graphics::Display;
using namespace winrt::Windows::UI::Core;

// Constants used to calculate screen rotations.
namespace ScreenRotation
{
    // 0-degree Z-rotation
    static const XMFLOAT4X4 Rotation0(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    // 90-degree Z-rotation
    static const XMFLOAT4X4 Rotation90(
        0.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    // 180-degree Z-rotation
    static const XMFLOAT4X4 Rotation180(
        -1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    // 270-degree Z-rotation
    static const XMFLOAT4X4 Rotation270(
        0.0f, -1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
};

// Constructor for DeviceResources.
DX::DeviceResources::DeviceResources() :
    m_screenViewport(),
    m_d3dFeatureLevel(D3D_FEATURE_LEVEL_9_1),
    m_d3dRenderTargetSize(),
    m_outputSize(),
    m_logicalSize(),
    m_nativeOrientation(DisplayOrientations::None),
    m_currentOrientation(DisplayOrientations::None),
    m_dpi(-1.0f),
    m_deviceNotify(nullptr)
{
    CreateDeviceIndependentResources();
    CreateDeviceResources();
}

// Configures resources that don't depend on the Direct3D device.
void DX::DeviceResources::CreateDeviceIndependentResources()
{
    // Initialize Direct2D resources.
    D2D1_FACTORY_OPTIONS options;
    ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
    // If the project is in a debug build, enable Direct2D debugging via SDK Layers.
    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

    // Initialize the Direct2D Factory.
    winrt::check_hresult(
        D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            __uuidof(m_d2dFactory),
            &options,
            m_d2dFactory.put_void()
        )
    );

    // Initialize the DirectWrite Factory.
    winrt::check_hresult(
        DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(m_dwriteFactory),
            reinterpret_cast<IUnknown**>(m_dwriteFactory.put())
        )
    );

    // Initialize the Windows Imaging Component (WIC) Factory.
    m_wicFactory = winrt::create_instance<IWICImagingFactory2>(CLSID_WICImagingFactory2);
}

// Configures the Direct3D device, and stores handles to it and the device context.
void DX::DeviceResources::CreateDeviceResources()
{
    // This flag adds support for surfaces with a different color channel ordering
    // than the API default. It is required for compatibility with Direct2D.
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
    if (DX::SdkLayersAvailable())
    {
        // If the project is in a debug build, enable debugging via SDK Layers with this flag.
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
    }
#endif

    // This array defines the set of DirectX hardware feature levels this app will support.
    // Note the ordering should be preserved.
    // Don't forget to declare your application's minimum required feature level in its
    // description. All applications are assumed to support 9.1 unless otherwise stated.
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    // Create the Direct3D 11 API device object and a corresponding context.
    winrt::com_ptr<ID3D11Device> device;
    winrt::com_ptr<ID3D11DeviceContext> context;

    HRESULT hr = D3D11CreateDevice(
        nullptr,                    // Specify nullptr to use the default adapter.
        D3D_DRIVER_TYPE_HARDWARE,   // Create a device using the hardware graphics driver.
        0,                          // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
        creationFlags,              // Set debug and Direct2D compatibility flags.
        featureLevels,              // List of feature levels this app can support.
        ARRAYSIZE(featureLevels),   // Size of the list above.
        D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Runtime apps.
        device.put(),               // Returns the Direct3D device created.
        &m_d3dFeatureLevel,         // Returns feature level of device created.
        context.put()                    // Returns the device immediate context.
    );

    if (FAILED(hr))
    {
        // If the initialization fails, fall back to the WARP device.
        // For more information on WARP, see: 
        // http://go.microsoft.com/fwlink/?LinkId=286690
        winrt::check_hresult(
            D3D11CreateDevice(
                nullptr,
                D3D_DRIVER_TYPE_WARP, // Create a WARP device instead of a hardware device.
                0,
                creationFlags,
                featureLevels,
                ARRAYSIZE(featureLevels),
                D3D11_SDK_VERSION,
                device.put(),
                &m_d3dFeatureLevel,
                context.put()
            )
        );
    }

    // Store pointers to the Direct3D 11.1 API device and immediate context.
    m_d3dDevice = device.as<ID3D11Device3>();

    m_d3dContext = context.as<ID3D11DeviceContext3>();

    // Create the Direct2D device object and a corresponding context.
    winrt::com_ptr<IDXGIDevice3> dxgiDevice;
    dxgiDevice = m_d3dDevice.as<IDXGIDevice3>();

    winrt::check_hresult(
        m_d2dFactory->CreateDevice(dxgiDevice.get(), m_d2dDevice.put())
    );

    winrt::check_hresult(
        m_d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            m_d2dContext.put()
        )
    );
}

// These resources need to be recreated every time the window size is changed.
void DX::DeviceResources::CreateWindowSizeDependentResources()
{
    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews[] = { nullptr };
    m_d3dContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
    m_d3dRenderTargetView = nullptr;
    m_d3dRenderTargetViewRight = nullptr;
    m_d2dContext->SetTarget(nullptr);
    m_d2dTargetBitmap = nullptr;
    m_d2dTargetBitmapRight = nullptr;
    m_d3dDepthStencilView = nullptr;
    m_d3dContext->Flush();

    // Calculate the necessary swap chain and render target size in pixels.
    m_outputSize.Width = DX::ConvertDipsToPixels(m_logicalSize.Width, m_dpi);
    m_outputSize.Height = DX::ConvertDipsToPixels(m_logicalSize.Height, m_dpi);

    // Prevent zero size DirectX content from being created.
    m_outputSize.Width = max(m_outputSize.Width, 1);
    m_outputSize.Height = max(m_outputSize.Height, 1);

    // The width and height of the swap chain must be based on the window's
    // natively-oriented width and height. If the window is not in the native
    // orientation, the dimensions must be reversed.
    DXGI_MODE_ROTATION displayRotation = ComputeDisplayRotation();

    bool swapDimensions = displayRotation == DXGI_MODE_ROTATION_ROTATE90 || displayRotation == DXGI_MODE_ROTATION_ROTATE270;
    m_d3dRenderTargetSize.Width = swapDimensions ? m_outputSize.Height : m_outputSize.Width;
    m_d3dRenderTargetSize.Height = swapDimensions ? m_outputSize.Width : m_outputSize.Height;

    if (m_swapChain != nullptr)
    {
        // If the swap chain already exists, resize it.
        HRESULT hr = m_swapChain->ResizeBuffers(
            2, // Double-buffered swap chain.
            static_cast<UINT>(m_d3dRenderTargetSize.Width),
            static_cast<UINT>(m_d3dRenderTargetSize.Height),
            DXGI_FORMAT_B8G8R8A8_UNORM,
            0
        );

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            HandleDeviceLost();

            // Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        } else
        {
            winrt::check_hresult(hr);
        }
    } else
    {
        CheckStereoEnabledStatus();

        // Otherwise, create a new one using the same adapter as the existing Direct3D device.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

        swapChainDesc.Width = static_cast<UINT>(m_d3dRenderTargetSize.Width); // Match the size of the window.
        swapChainDesc.Height = static_cast<UINT>(m_d3dRenderTargetSize.Height);
        swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // This is the most common swap chain format.
        swapChainDesc.Stereo = m_stereoEnabled;
        swapChainDesc.SampleDesc.Count = 1; // Don't use multi-sampling.
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2; // Use double-buffering to minimize latency.
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use this SwapEffect.
        swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_FOREGROUND_LAYER;
        swapChainDesc.Scaling = DXGI_SCALING_NONE;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

        // This sequence obtains the DXGI factory that was used to create the Direct3D device above.
        winrt::com_ptr<IDXGIDevice3> dxgiDevice;
        dxgiDevice = m_d3dDevice.as<IDXGIDevice3>();

        winrt::com_ptr<IDXGIAdapter> dxgiAdapter;
        winrt::check_hresult(
            dxgiDevice->GetAdapter(dxgiAdapter.put())
        );

        winrt::com_ptr<IDXGIFactory3> dxgiFactory;
        winrt::check_hresult(
            dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.put()))
        );

        winrt::check_hresult(
            dxgiFactory->CreateSwapChainForCoreWindow(
                m_d3dDevice.get(),
                winrt::get_unknown(m_window.get()),
                &swapChainDesc,
                nullptr,
                m_swapChain.put()
            )
        );

        // Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
        // ensures that the application will only render after each VSync, minimizing power consumption.
        winrt::check_hresult(
            dxgiDevice->SetMaximumFrameLatency(1)
        );
    }

    // Set the proper orientation for the swap chain, and generate 2D and
    // 3D matrix transformations for rendering to the rotated swap chain.
    // Note the rotation angle for the 2D and 3D transforms are different.
    // This is due to the difference in coordinate spaces. Additionally,
    // the 3D matrix is specified explicitly to avoid rounding errors.

    switch (displayRotation)
    {
    case DXGI_MODE_ROTATION_IDENTITY:
        m_orientationTransform2D = Matrix3x2F::Identity();
        m_orientationTransform3D = ScreenRotation::Rotation0;
        break;

    case DXGI_MODE_ROTATION_ROTATE90:
        m_orientationTransform2D =
            Matrix3x2F::Rotation(90.0f) *
            Matrix3x2F::Translation(m_logicalSize.Height, 0.0f);
        m_orientationTransform3D = ScreenRotation::Rotation270;
        break;

    case DXGI_MODE_ROTATION_ROTATE180:
        m_orientationTransform2D =
            Matrix3x2F::Rotation(180.0f) *
            Matrix3x2F::Translation(m_logicalSize.Width, m_logicalSize.Height);
        m_orientationTransform3D = ScreenRotation::Rotation180;
        break;

    case DXGI_MODE_ROTATION_ROTATE270:
        m_orientationTransform2D =
            Matrix3x2F::Rotation(270.0f) *
            Matrix3x2F::Translation(0.0f, m_logicalSize.Width);
        m_orientationTransform3D = ScreenRotation::Rotation90;
        break;

    default:
        winrt::throw_hresult(E_FAIL);
    }

    winrt::check_hresult(
        m_swapChain->SetRotation(displayRotation)
    );

    // Create a render target view of the swap chain back buffer.
    winrt::com_ptr<ID3D11Texture2D> backBuffer = winrt::capture<ID3D11Texture2D>(m_swapChain, &IDXGISwapChain1::GetBuffer, 0);

    winrt::check_hresult(
        m_d3dDevice->CreateRenderTargetView(
            backBuffer.get(),
            nullptr,
            m_d3dRenderTargetView.put()
        )
    );

    // Stereo swapchains have an arrayed resource, so create a second Render Target
    // for the right eye buffer.
    if (m_stereoEnabled)
    {
        CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewRightDesc(
            D3D11_RTV_DIMENSION_TEXTURE2DARRAY,
            DXGI_FORMAT_B8G8R8A8_UNORM,
            0,
            1,
            1
        );

        winrt::check_hresult(
            m_d3dDevice->CreateRenderTargetView(
                backBuffer.get(),
                &renderTargetViewRightDesc,
                m_d3dRenderTargetViewRight.put()
            )
        );
    }

    // Create a depth stencil view for use with 3D rendering if needed.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(
        DXGI_FORMAT_D24_UNORM_S8_UINT,
        static_cast<UINT>(m_d3dRenderTargetSize.Width),
        static_cast<UINT>(m_d3dRenderTargetSize.Height),
        1, // This depth stencil view has only one texture.
        1, // Use a single mipmap level.
        D3D11_BIND_DEPTH_STENCIL
    );

    winrt::com_ptr<ID3D11Texture2D> depthStencil;
    winrt::check_hresult(
        m_d3dDevice->CreateTexture2D(
            &depthStencilDesc,
            nullptr,
            depthStencil.put()
        )
    );

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    winrt::check_hresult(
        m_d3dDevice->CreateDepthStencilView(
            depthStencil.get(),
            &depthStencilViewDesc,
            m_d3dDepthStencilView.put()
        )
    );

    // Set the 3D rendering viewport to target the entire window.
    m_screenViewport = CD3D11_VIEWPORT(
        0.0f,
        0.0f,
        m_d3dRenderTargetSize.Width,
        m_d3dRenderTargetSize.Height
    );

    m_d3dContext->RSSetViewports(1, &m_screenViewport);

    // Create a Direct2D target bitmap associated with the
    // swap chain back buffer and set it as the current target.
    D2D1_BITMAP_PROPERTIES1 bitmapProperties =
        D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED
            ),
            m_dpi,
            m_dpi
        );

    winrt::com_ptr<IDXGIResource1> dxgiBackBuffer = winrt::capture<IDXGIResource1>(m_swapChain, &IDXGISwapChain1::GetBuffer, 0);

    winrt::com_ptr<IDXGISurface2> dxgiSurface;
    winrt::check_hresult(
        dxgiBackBuffer->CreateSubresourceSurface(0, dxgiSurface.put())
    );

    winrt::check_hresult(
        m_d2dContext->CreateBitmapFromDxgiSurface(
            dxgiSurface.get(),
            &bitmapProperties,
            m_d2dTargetBitmap.put()
        )
    );

    // Stereo swapchains have an arrayed resource, so create a second Target Bitmap
    // for the right eye buffer.
    if (m_stereoEnabled)
    {
        winrt::check_hresult(
            dxgiBackBuffer->CreateSubresourceSurface(1, dxgiSurface.put())
        );
        winrt::check_hresult(
            m_d2dContext->CreateBitmapFromDxgiSurface(
                dxgiSurface.get(),
                &bitmapProperties,
                m_d2dTargetBitmapRight.put()
            )
        );
    }

    m_d2dContext->SetTarget(m_d2dTargetBitmap.get());

    // Grayscale text anti-aliasing is recommended for all Windows Store apps.
    m_d2dContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
}

// This method is called when the CoreWindow is created (or re-created).
void DX::DeviceResources::SetWindow(CoreWindow const& window)
{
    DisplayInformation currentDisplayInformation = DisplayInformation::GetForCurrentView();

    m_window = window;
    m_logicalSize = winrt::Windows::Foundation::Size(window.Bounds().Width, window.Bounds().Height);
    m_nativeOrientation = currentDisplayInformation.NativeOrientation();
    m_currentOrientation = currentDisplayInformation.CurrentOrientation();
    m_dpi = currentDisplayInformation.LogicalDpi();
    m_d2dContext->SetDpi(m_dpi, m_dpi);

    CreateWindowSizeDependentResources();
}

// This method is called in the event handler for the SizeChanged event.
void DX::DeviceResources::SetLogicalSize(winrt::Windows::Foundation::Size logicalSize)
{
    if (m_logicalSize != logicalSize)
    {
        m_logicalSize = logicalSize;
        CreateWindowSizeDependentResources();
    }
}

// This method is called in the event handler for the DpiChanged event.
void DX::DeviceResources::SetDpi(float dpi)
{
    if (dpi != m_dpi)
    {
        m_dpi = dpi;

        // When the display DPI changes, the logical size of the window (measured in Dips) also changes and needs to be updated.
        m_logicalSize = winrt::Windows::Foundation::Size(m_window.get().Bounds().Width, m_window.get().Bounds().Height);

        m_d2dContext->SetDpi(m_dpi, m_dpi);
        CreateWindowSizeDependentResources();
    }
}

// This method is called in the event handler for the OrientationChanged event.
void DX::DeviceResources::SetCurrentOrientation(DisplayOrientations currentOrientation)
{
    if (m_currentOrientation != currentOrientation)
    {
        m_currentOrientation = currentOrientation;
        CreateWindowSizeDependentResources();
    }
}

// This method is called in the event handler for the DisplayContentsInvalidated event.
void DX::DeviceResources::ValidateDevice()
{
    // The D3D Device is no longer valid if the default adapter changed since the device
    // was created or if the device has been removed.

    // First, get the information for the default adapter from when the device was created.

    winrt::com_ptr<IDXGIDevice3> dxgiDevice;
    dxgiDevice = m_d3dDevice.as<IDXGIDevice3>();

    winrt::com_ptr<IDXGIAdapter> deviceAdapter;
    winrt::check_hresult(dxgiDevice->GetAdapter(deviceAdapter.put()));

    winrt::com_ptr<IDXGIFactory2> deviceFactory;
    winrt::check_hresult(deviceAdapter->GetParent(IID_PPV_ARGS(deviceFactory.put())));

    winrt::com_ptr<IDXGIAdapter1> previousDefaultAdapter;
    winrt::check_hresult(deviceFactory->EnumAdapters1(0, previousDefaultAdapter.put()));

    DXGI_ADAPTER_DESC previousDesc;
    winrt::check_hresult(previousDefaultAdapter->GetDesc(&previousDesc));

    // Next, get the information for the current default adapter.

    winrt::com_ptr<IDXGIFactory2> currentFactory;
    winrt::check_hresult(CreateDXGIFactory1(IID_PPV_ARGS(currentFactory.put())));

    winrt::com_ptr<IDXGIAdapter1> currentDefaultAdapter;
    winrt::check_hresult(currentFactory->EnumAdapters1(0, currentDefaultAdapter.put()));

    DXGI_ADAPTER_DESC currentDesc;
    winrt::check_hresult(currentDefaultAdapter->GetDesc(&currentDesc));

    // If the adapter LUIDs don't match, or if the device reports that it has been removed,
    // a new D3D device must be created.

    if (previousDesc.AdapterLuid.LowPart != currentDesc.AdapterLuid.LowPart ||
        previousDesc.AdapterLuid.HighPart != currentDesc.AdapterLuid.HighPart ||
        FAILED(m_d3dDevice->GetDeviceRemovedReason()))
    {
        // Release references to resources related to the old device.
        dxgiDevice = nullptr;
        deviceAdapter = nullptr;
        deviceFactory = nullptr;
        previousDefaultAdapter = nullptr;

        // Create a new device and swap chain.
        HandleDeviceLost();
    }
}

// Recreate all device resources and set them back to the current state.
void DX::DeviceResources::HandleDeviceLost()
{
    m_swapChain = nullptr;

    if (m_deviceNotify != nullptr)
    {
        m_deviceNotify->OnDeviceLost();
    }

    // Make sure the rendering state has been released.
    m_d3dContext->OMSetRenderTargets(0, nullptr, nullptr);
    m_d3dDepthStencilView = nullptr;
    m_d3dRenderTargetView = nullptr;
    m_d3dRenderTargetViewRight = nullptr;

    m_d2dContext->SetTarget(nullptr);
    m_d2dTargetBitmap = nullptr;
    m_d2dTargetBitmapRight = nullptr;
    m_d2dContext = nullptr;
    m_d2dDevice = nullptr;

    m_d3dContext->Flush();

    CreateDeviceResources();
    m_d2dContext->SetDpi(m_dpi, m_dpi);
    CreateWindowSizeDependentResources();

    if (m_deviceNotify != nullptr)
    {
        m_deviceNotify->OnDeviceRestored();
    }
}

// Register our DeviceNotify to be informed on device lost and creation.
void DX::DeviceResources::RegisterDeviceNotify(DX::IDeviceNotify* deviceNotify)
{
    m_deviceNotify = deviceNotify;
}

// Call this method when the app suspends. It provides a hint to the driver that the app 
// is entering an idle state and that temporary buffers can be reclaimed for use by other apps.
void DX::DeviceResources::Trim()
{
    winrt::com_ptr<IDXGIDevice3> dxgiDevice;
    dxgiDevice = m_d3dDevice.as<IDXGIDevice3>();

    dxgiDevice->Trim();
}

// Present the contents of the swap chain to the screen.
void DX::DeviceResources::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0);

    // Discard the contents of the render target.
    // This is a valid operation only when the existing contents will be entirely
    // overwritten. If dirty or scroll rects are used, this call should be removed.
    m_d3dContext->DiscardView(m_d3dRenderTargetView.get());

    // Discard the contents of the depth stencil.
    m_d3dContext->DiscardView(m_d3dDepthStencilView.get());

    // If the device was removed either by a disconnection or a driver upgrade, we 
    // must recreate all device resources.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        HandleDeviceLost();
    } else
    {
        winrt::check_hresult(hr);
    }
}

// This method determines the rotation between the display device's native Orientation and the
// current display orientation.
DXGI_MODE_ROTATION DX::DeviceResources::ComputeDisplayRotation()
{
    DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;

    // Note: NativeOrientation can only be Landscape or Portrait even though
    // the DisplayOrientations enum has other values.
    switch (m_nativeOrientation)
    {
    case DisplayOrientations::Landscape:
        switch (m_currentOrientation)
        {
        case DisplayOrientations::Landscape:
            rotation = DXGI_MODE_ROTATION_IDENTITY;
            break;

        case DisplayOrientations::Portrait:
            rotation = DXGI_MODE_ROTATION_ROTATE270;
            break;

        case DisplayOrientations::LandscapeFlipped:
            rotation = DXGI_MODE_ROTATION_ROTATE180;
            break;

        case DisplayOrientations::PortraitFlipped:
            rotation = DXGI_MODE_ROTATION_ROTATE90;
            break;
        }
        break;

    case DisplayOrientations::Portrait:
        switch (m_currentOrientation)
        {
        case DisplayOrientations::Landscape:
            rotation = DXGI_MODE_ROTATION_ROTATE90;
            break;

        case DisplayOrientations::Portrait:
            rotation = DXGI_MODE_ROTATION_IDENTITY;
            break;

        case DisplayOrientations::LandscapeFlipped:
            rotation = DXGI_MODE_ROTATION_ROTATE270;
            break;

        case DisplayOrientations::PortraitFlipped:
            rotation = DXGI_MODE_ROTATION_ROTATE180;
            break;
        }
        break;
    }
    return rotation;
}

void DX::DeviceResources::UpdateStereoState()
{
    bool previousStereoState = m_stereoEnabled;
    CheckStereoEnabledStatus();
    if (previousStereoState != m_stereoEnabled)
    {
        // Swap chain needs to be recreated so release the existing one.
        // The rest of the dependent resources with be released in CreateWindowSizeDependentResources.
        m_swapChain = nullptr;
        CreateWindowSizeDependentResources();
    }

}

void DX::DeviceResources::CheckStereoEnabledStatus()
{
    // first, retrieve the underlying DXGI Device from the D3D Device
    winrt::com_ptr<IDXGIDevice1> dxgiDevice;
    dxgiDevice = m_d3dDevice.as<IDXGIDevice1>();

    // next, get the associated adapter from the DXGI Device
    winrt::com_ptr<IDXGIAdapter> dxgiAdapter;
    winrt::check_hresult(
        dxgiDevice->GetAdapter(dxgiAdapter.put())
    );

    // next, get the parent factory from the DXGI adapter
    winrt::com_ptr<IDXGIFactory2> dxgiFactory;
    winrt::check_hresult(
        dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.put()))
    );

    m_stereoEnabled = dxgiFactory->IsWindowedStereoEnabled() ? true : false;
}