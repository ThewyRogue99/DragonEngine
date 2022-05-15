#pragma once

#include "Window.h"
#include "Engine/Events/ApplicationEvent.h"
#include "LayerStack.h"

#include "Engine/ImGui/ImGuiLayer.h"

namespace Engine
{
	class Application
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
		bool OnWindowResize(WindowResizeEvent& event);

		Scope<Window> AppWindow;
		ImGuiLayer* AppImGuiLayer;

		bool bIsRunning = false;

		Timestep DeltaTime;
		LayerStack layerStack;

		float LastFrameTime = 0.f;

		bool bIsMinimized = false;

		static Application* Instance;
	};

	Application* CreateApplication();
};