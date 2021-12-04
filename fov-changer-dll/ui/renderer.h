#pragma once

#include "../pch.h"

#include "main.h"
#include "common/DeviceResources.h"
#include "common/DirectXHelper.h"
#include "../utils/Logger.h"

using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::ApplicationModel::Core;


namespace UI
{
	// Needed files from other headers
	class Main;

	void RenderThread();

	// Renders
	class Renderer
	{
	public:
		Main*									m_ui;
		winrt::agile_ref<CoreWindow>			m_window;
		std::shared_ptr<DX::DeviceResources>	m_deviceResources;

		bool									m_renderNeeded = true;
		bool									m_visible = true;

		winrt::com_ptr<ID2D1SolidColorBrush>	m_whiteBrush;
		winrt::com_ptr<ID2D1SolidColorBrush>	m_blackBrush;
		winrt::com_ptr<IDWriteTextLayout>		m_dwriteTextLayout;
		winrt::com_ptr<IDWriteTextFormat>		m_dwriteTextFormat;

		winrt::com_ptr<ID2D1Effect>				m_dropShadowEffect;

		winrt::com_ptr<ID2D1Bitmap>				m_bitmapTest;
		winrt::com_ptr<ID2D1Bitmap1>			m_bitmap;

		bool									m_test = true;

	public:
		Renderer();

		void CreateDeviceDependentResources();
		void CreateDeviceIndependentResources();
		void Render();
	};
}