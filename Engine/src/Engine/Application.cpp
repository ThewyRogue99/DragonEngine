#include "depch.h"

#include "Application.h"

#include "Events/ApplicationEvent.h"

#include <GLFW/glfw3.h>

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
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			AppWindow->OnUpdate();
		}
	}

}