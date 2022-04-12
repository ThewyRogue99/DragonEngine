#pragma once

#include "Window.h"
#include "Events/ApplicationEvent.h"
#include "LayerStack.h"

namespace Engine
{
	class ENGINE_API Application
	{
	public:
		Application();

		virtual ~Application();

		void Run();

		void OnEvent(Event& event);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline static Application& Get() { return *Instance; }
		inline Window& GetWindow() { return *AppWindow; }

	private:
		bool OnWindowClose(WindowCloseEvent& event);

		std::unique_ptr<Window> AppWindow;

		bool bIsRunning = false;

		LayerStack layerStack;

		static Application* Instance;
	};

	Application* CreateApplication();
}