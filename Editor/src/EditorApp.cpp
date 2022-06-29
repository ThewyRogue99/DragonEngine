#include <Engine/Core/Application.h>
#include <Engine/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Engine
{
	class EngineEditor : public Application
	{
	public:
		EngineEditor(ApplicationCommandLineArgs args) : Application("Dragon Engine Editor", args)
		{
			PushLayer(new EditorLayer());
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new EngineEditor(args);
	}
}
