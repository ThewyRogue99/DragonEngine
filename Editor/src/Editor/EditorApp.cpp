#include <Engine/Core/ApplicationCore.h>

#include "Debug/EditorConsole.h"

#include "Editor/Tools/EditorTool.h"
#include "EditorLayer.h"
#include "ImGui/ImGuiLayer.h"

#include "Engine/Debug/Instrumentor.h"

namespace Engine
{
	class EditorApp : public Application
	{
	public:
		EditorApp(const ApplicationSpecification& Specs) : Application(Specs)
		{
			AppImGuiLayer = new ImGuiLayer();
			PushOverlay(AppImGuiLayer);

			AppEditorLayer = new EditorLayer();
			PushLayer(AppEditorLayer);
			EditorTool::SetEditorLayer(AppEditorLayer);
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
