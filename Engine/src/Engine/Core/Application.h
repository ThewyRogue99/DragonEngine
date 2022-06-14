#pragma once

#include "Window.h"
#include "Engine/Events/ApplicationEvent.h"
#include "LayerStack.h"

#include "Engine/ImGui/ImGuiLayer.h"

namespace Engine
{
	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			return Args[index];
		}
	};

	class Application
	{
	public:
		Application(const std::string& name = "Dragon Engine App", ApplicationCommandLineArgs args = ApplicationCommandLineArgs());

		virtual ~Application();

		void Run();

		void OnEvent(Event& event);

		void Close();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline static Application& Get() { return *Instance; }
		inline Window& GetWindow() { return *AppWindow; }

		ApplicationCommandLineArgs GetCommandLineArgs() const { return CommandLineArgs; }

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

		ApplicationCommandLineArgs CommandLineArgs;

		static Application* Instance;
	};

	Application* CreateApplication(ApplicationCommandLineArgs args);
};