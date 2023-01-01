#include <Engine/Core/Application.h>
#include <Engine/Core/EntryPoint.h>

#include "Debug/EditorConsole.h"

#include "EditorLayer.h"
#include "ImGui/ImGuiLayer.h"

namespace Engine
{
	class EngineEditor : public Application
	{
	public:
		EngineEditor(const ApplicationSpecification& Specs) : Application(Specs)
		{
			PushLayer(new EditorLayer());

			AppImGuiLayer = new ImGuiLayer();
			PushOverlay(AppImGuiLayer);
		}

	protected:
		virtual void OnUpdate(float DeltaTime) override
		{
			{
				DE_PROFILE_SCOPE("UI Render");
				Application::OnUpdate(DeltaTime);

				AppImGuiLayer->Begin();

				for (Layer* layer : layerStack)
					layer->OnImGuiRender(DeltaTime);

				AppImGuiLayer->End();
			}
		}

	private:
		ImGuiLayer* AppImGuiLayer = nullptr;
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification Specs;
		Specs.Name = TEXT("Dragon Engine");
		Specs.CommandLineArgs = args;

		Ref<EditorConsole> console = CreateRef<EditorConsole>();
		Log::SetConsole(console);

		return new EngineEditor(Specs);
	}
}
