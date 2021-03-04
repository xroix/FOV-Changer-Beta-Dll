#pragma once

#include "../pch.h"

#include "common/DeviceResources.h"
#include "common/DirectXHelper.h"
#include "../utils/Logger.h";


using namespace winrt::Windows::UI::Core;


namespace UI
{
	class Renderer
	{
	public:
		Renderer();

		void Setup(winrt::agile_ref<CoreWindow> window);
		void Loop();

	public:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		winrt::com_ptr<ID2D1SolidColorBrush> m_textBrush;
		winrt::com_ptr<IDWriteTextLayout>  m_dwriteTextLayout;
		winrt::com_ptr<IDWriteTextFormat>  m_dwriteTextFormat;
	};

	void RenderingThread(const std::shared_ptr<UI::Renderer> renderer, CoreWindow const& window);
}