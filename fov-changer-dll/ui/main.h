#pragma once

#include "../pch.h"

#include "renderer.h"
#include "../utils/Logger.h"

using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::ApplicationModel::Core;


namespace UI
{
	class Renderer;

	// Contains loop
	class Main
	{
	public:
		Main();

		void StartRenderThread(std::shared_ptr<UI::Main> ui);
		void SetWindow(winrt::agile_ref<CoreWindow> window);
		void Loop();

		void OnVisibilityChanged(CoreWindow const&, VisibilityChangedEventArgs const& args);

	public:
		winrt::agile_ref<CoreWindow>			m_window;
		std::thread								m_renderThread;
		std::shared_ptr<UI::Renderer>			m_renderer = nullptr;
		bool									m_visible = true;
	};
}