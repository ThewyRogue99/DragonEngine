#include "depch.h"

#include "Application.h"

#include "Events/ApplicationEvent.h"

namespace Engine
{
	Application::Application()
	{
		AppWindow = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		bIsRunning = true;

		while (bIsRunning)
		{
			AppWindow->OnUpdate();
		}
	}

}