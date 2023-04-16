#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "Window.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Core/LayerStack.h"

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

	struct ApplicationSpecification
	{
		WString Name = TEXT("Dragon Engine App");
		WString WorkingDirectory = WString();
		ApplicationCommandLineArgs CommandLineArgs = ApplicationCommandLineArgs();
	};

	class Application
	{
	public:
		ENGINE_API Application(const ApplicationSpecification& Specs);

		ENGINE_API virtual ~Application();

		ENGINE_API void Run();

		ENGINE_API void OnEvent(Event& event);

		ENGINE_API void Close();

		ENGINE_API void PushLayer(Layer* layer);
		ENGINE_API void PushOverlay(Layer* layer);

		float GetDeltaTime() const { return DeltaTime; }

		inline static Application& Get() { return *Instance; }
		inline Ref<Window> GetWindow() { return AppWindow; }

		const ApplicationSpecification& GetSpecification() const { return AppSpecification; }

	protected:
		ENGINE_API virtual void OnUpdate(float DeltaTime);

	private:
		ENGINE_API bool OnWindowClose(WindowCloseEvent& event);
		ENGINE_API bool OnWindowResize(WindowResizeEvent& event);

	protected:
		Ref<Window> AppWindow;

		bool bIsRunning = false;

		float DeltaTime = 0.f;
		LayerStack layerStack;

		float LastFrameTime = 0.f;

		bool bIsMinimized = false;

		ApplicationSpecification AppSpecification;

		ENGINE_API static Application* Instance;
	};

	Application* CreateApplication(ApplicationCommandLineArgs args);
};