#include "depch.h"

#include "ApplicationCore.h"
#include "Platform/Platform.h"

namespace Engine
{
	Application* Application::Instance = nullptr;

	Application::Application()
	{
		DE_CORE_ASSERT(!Instance, "Application already exists");
		Instance = this;

		AppWindow = Scope<Window>(Window::Create());
		AppWindow->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();

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
		d.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

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
			float time = Platform::GetTime();

			DeltaTime = time - LastFrameTime;
			LastFrameTime = time;

			if (!bIsMinimized)
			{
				for (Layer* layer : layerStack)
					layer->OnUpdate(DeltaTime);
			}

			AppImGuiLayer->Begin();

			for (Layer* layer : layerStack)
				layer->OnImGuiRender(DeltaTime);

			AppImGuiLayer->End();

			AppWindow->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		bIsRunning = false;

		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& event)
	{
		unsigned int width = event.GetWidth(), height = event.GetHeight();

		if (width == 0 || height == 0)
			bIsMinimized = true;
		else
			bIsMinimized = false;

		Renderer::OnWindowResize(width, height);

		return true;
	}
}