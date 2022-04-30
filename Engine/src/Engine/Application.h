#pragma once

#include "Window.h"
#include "Events/ApplicationEvent.h"
#include "LayerStack.h"

#include "ImGui/ImGuiLayer.h"

#include "Core/Timestep.h"

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

		Scope<Window> AppWindow;
		ImGuiLayer* AppImGuiLayer;

		bool bIsRunning = false;

		Timestep DeltaTime;
		LayerStack layerStack;

		float LastFrameTime = 0.f;

		static Application* Instance;
	};

	Application* CreateApplication();
}