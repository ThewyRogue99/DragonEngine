#pragma once

#include "Window.h"

namespace Engine
{
	class ENGINE_API Application
	{
	public:
		Application();

		virtual ~Application();

		void Run();

	private:
		std::unique_ptr<Window> AppWindow;

		bool bIsRunning = false;
	};

	Application* CreateApplication();
}