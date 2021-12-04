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
		winrt::agile_ref<CoreWindow>		m_window;
		CoreDispatcher						m_dispatcher = nullptr;

		std::thread							m_renderThread;
		std::shared_ptr<UI::Renderer>		m_renderer = nullptr;
		bool								m_visible = true;

	public:
		Main(const Main& other) = delete;
		static Main& Get() { static Main instance; return instance; }

		void StartRenderThread();
		void SetWindow(winrt::agile_ref<CoreWindow> window);
		void Loop();

		// Run things as real native UI
		void InvokeAsUIThread(DispatchedHandler const& agileCallback);
		void InvokeAsUIThreadAsync(DispatchedHandler const& agileCallback);

		// Events
		void OnVisibilityChanged(IInspectable const&, VisibilityChangedEventArgs const& args);
		void OnSizeChanged(IInspectable const&, WindowSizeChangedEventArgs const& args);

	private:
		Main();
	};
}