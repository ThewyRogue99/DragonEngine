#include "depch.h"

#include "Application.h"

#include <glad/glad.h>

namespace Engine
{
	Application* Application::Instance = nullptr;

	Application::Application()
	{
		DE_CORE_ASSERT(!Instance, "Application already exists");
		Instance = this;

		AppWindow = std::unique_ptr<Window>(Window::Create());
		AppWindow->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		AppImGuiLayer = new ImGuiLayer();
		PushOverlay(AppImGuiLayer);
	}

	Application::~Application()
	{
		
	}

	void Application::PushLayer(Layer* layer)
	{
		layerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		layerStack.PushOverlay(layer);
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher d(event);
		d.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

		for (auto it = layerStack.end(); it != layerStack.begin();)
		{
			(*--it)->OnEvent(event);

			if (event.IsHandled())
				break;
		}
	}

	void Application::Run()
	{
		bIsRunning = true;

		while (bIsRunning)
		{
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			for (Layer* layer : layerStack)
				layer->OnUpdate();

			AppImGuiLayer->Begin();

			for (Layer* layer : layerStack)
				layer->OnImGuiRender();

			AppImGuiLayer->End();

			AppWindow->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		bIsRunning = false;

		return true;
	}
}