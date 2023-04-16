#include "depch.h"
#include "Application.h"

#include "Engine/Debug/Instrumentor.h"

#include "Engine/Renderer/Renderer2D.h"
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

		bIsRunning = true;

		CString NameUTF8 = TypeUtils::FromUTF16(AppSpecification.Name.c_str());

		if (!AppSpecification.WorkingDirectory.empty())
			std::filesystem::current_path(AppSpecification.WorkingDirectory);

		Renderer::Init();

		AppWindow = Window::Create(WindowProps(NameUTF8.c_str()));
		AppWindow->OnEvent().AddCallback(BIND_CLASS_FN(Application::OnEvent));

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
			(*(--it))->OnEvent(event);

			if (event.IsHandled())
				break;
		}
	}

	void Application::Close()
	{
		if (bIsRunning)
		{
			bIsRunning = false;

			AudioEngine::Shutdown();
			ScriptEngine::Shutdown();

			Renderer2D::Shutdown();
			Renderer::Shutdown();

			DE_WARN(
				Application, "Shutting down Application: {0}",
				TypeUtils::FromUTF16(AppSpecification.Name).c_str()
			);
		}
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

		Renderer::GetRendererAPI()->SetViewport(0, 0, width, height);

		return true;
	}
}