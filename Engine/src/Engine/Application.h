#pragma once

#include "Window.h"
#include "Events/ApplicationEvent.h"
#include "LayerStack.h"

#include "ImGui/ImGuiLayer.h"

#include "Renderer/Shader.h"

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
		ImGuiLayer* AppImGuiLayer;

		bool bIsRunning = false;

		LayerStack layerStack;

		unsigned int VAO = 0, VBO = 0, EBO = 0;
		
		std::unique_ptr<Shader> AppShader;

		static Application* Instance;
	};

	Application* CreateApplication();
}