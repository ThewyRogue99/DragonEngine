#include "depch.h"
#include "Application.h"

#include "Engine/Debug/Debug.h"

#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Platform/Platform.h"

#include "Engine/Audio/AudioEngine.h"
#include "Engine/Scripting/ScriptEngine.h"

#include <filesystem>

namespace Engine
{
	Application* Application::Instance = nullptr;

	Application::Application(const ApplicationSpecification& Specs) : AppSpecification(Specs)
	{
		DE_PROFILE_FUNCTION();

		DE_ASSERT(!Instance, "Application already exists");
		Instance = this;

		std::string NameUTF8 = TypeUtils::FromUTF16(AppSpecification.Name.c_str());

		if (!AppSpecification.WorkingDirectory.empty())
			std::filesystem::current_path(AppSpecification.WorkingDirectory);

		RenderCommand::Init();

		AppWindow = Scope<Window>(Window::Create(WindowProps(NameUTF8.c_str())));
		AppWindow->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Renderer2D::Init();
		ScriptEngine::Init();
		AudioEngine::Init();

		DE_INFO(Application, "Initialized Application");
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

	void Application::Close()
	{
		bIsRunning = false;

		AudioEngine::Shutdown();
		ScriptEngine::Shutdown();

		DE_WARN(
			Application, "Shutting down Application: {0}",
			TypeUtils::FromUTF16(AppSpecification.Name).c_str()
		);
	}

	void Application::Run()
	{
		DE_PROFILE_FUNCTION();

		bIsRunning = true;

		while (bIsRunning)
		{
			float time = Platform::GetTime();

			DeltaTime = time - LastFrameTime;
			LastFrameTime = time;

			if (!bIsMinimized)
			{
				{
					DE_PROFILE_SCOPE("Layers Update");

					for (Layer* layer : layerStack)
						layer->OnUpdate(DeltaTime);
				}
			}

			OnUpdate(DeltaTime);

			AppWindow->OnUpdate();
		}
	}

	void Application::OnUpdate(float DeltaTime)
	{
		
	}

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		Close();

		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& event)
	{
		unsigned int width = event.GetWidth(), height = event.GetHeight();

		if (width == 0 || height == 0)
			bIsMinimized = true;
		else
			bIsMinimized = false;

		RenderCommand::SetViewport(0, 0, width, height);

		return true;
	}
}