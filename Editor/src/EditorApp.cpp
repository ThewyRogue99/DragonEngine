#include <Engine/Core/Application.h>
#include <Engine/Core/EntryPoint.h>

#include "Debug/EditorConsole.h"

#include "EditorLayer.h"

namespace Engine
{
	class EngineEditor : public Application
	{
	public:
		EngineEditor(const ApplicationSpecification& Specs) : Application(Specs)
		{
			PushLayer(new EditorLayer());
		}
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
