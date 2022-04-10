#pragma once

#include "Window.h"

#include "Events/ApplicationEvent.h"

namespace Engine
{
	class ENGINE_API Application
	{
	public:
		Application();

		virtual ~Application();

		void Run();

		void OnEvent(Event& event);

	private:
		bool OnWindowClose(WindowCloseEvent& event);

		std::unique_ptr<Window> AppWindow;

		bool bIsRunning = false;
	};

	Application* CreateApplication();
}