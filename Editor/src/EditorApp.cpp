#include <Engine/Core/Application.h>
#include <Engine/Core/EntryPoint.h>

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

		return new EngineEditor(Specs);
	}
}
