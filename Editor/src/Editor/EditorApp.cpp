#include <Engine/Core/Application.h>
#include <Engine/Core/EntryPoint.h>

#include "Debug/EditorConsole.h"

#include "Editor/Tools/EditorTool.h"
#include "EditorLayer.h"
#include "ImGui/ImGuiLayer.h"

namespace Engine
{
	class EditorApp : public Application
	{
	public:
		EditorApp(const ApplicationSpecification& Specs) : Application(Specs)
		{
			AppEditorLayer = new EditorLayer();
			PushLayer(AppEditorLayer);
			EditorTool::SetEditorLayer(AppEditorLayer);

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

				AppEditorLayer->OnImGuiRender(DeltaTime);

				AppImGuiLayer->End();
			}
		}

	private:
		ImGuiLayer* AppImGuiLayer = nullptr;
		EditorLayer* AppEditorLayer = nullptr;
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification Specs;
		Specs.Name = TEXT("Dragon Engine");
		Specs.CommandLineArgs = args;

		Ref<EditorConsole> console = CreateRef<EditorConsole>();
		Log::SetConsole(console);

		return new EditorApp(Specs);
	}
}
