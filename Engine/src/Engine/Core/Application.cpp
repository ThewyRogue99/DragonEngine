#include "depch.h"
#include "Application.h"

#include "Engine/Renderer/Renderer.h"
#include "Platform/Platform.h"

#include <filesystem>

namespace Engine
{
	Application* Application::Instance = nullptr;

	Application::Application(const ApplicationSpecification& Specs) : AppSpecification(Specs)
	{
		DE_PROFILE_FUNCTION();

		DE_CORE_ASSERT(!Instance, "Application already exists");
		Instance = this;

		std::string NameUTF8 = TypeUtils::FromUTF16(AppSpecification.Name.c_str());

		if (!AppSpecification.WorkingDirectory.empty())
			std::filesystem::current_path(AppSpecification.WorkingDirectory);

		AppWindow = CreateScope<Window>(Window::Create(WindowProps(NameUTF8.c_str())));
		AppWindow->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		{
			DE_PROFILE_SCOPE("Renderer Init");

			Renderer::Init();
		}

		{
			DE_PROFILE_SCOPE("UI Layer Attach");
			AppImGuiLayer = new ImGuiLayer();
			PushOverlay(AppImGuiLayer);
		}
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

			{
				DE_PROFILE_SCOPE("UI Render");

				AppImGuiLayer->Begin();

				for (Layer* layer : layerStack)
					layer->OnImGuiRender(DeltaTime);

				AppImGuiLayer->End();
			}

			AppWindow->OnUpdate();
		}
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

		Renderer::OnWindowResize(width, height);

		return true;
	}
}