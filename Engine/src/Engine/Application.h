#pragma once

#include "Window.h"
#include "Events/ApplicationEvent.h"
#include "LayerStack.h"

#include "ImGui/ImGuiLayer.h"

#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"

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
		
		std::unique_ptr<Shader> AppShader;
		std::unique_ptr<Shader> CubeShader;

		std::shared_ptr<VertexArray> vArray;

		std::shared_ptr<VertexBuffer> AppVertexBuffer;
		std::shared_ptr<IndexBuffer> AppIndexBuffer;
		std::shared_ptr<VertexArray> AppVertexArray;

		static Application* Instance;
	};

	Application* CreateApplication();
}