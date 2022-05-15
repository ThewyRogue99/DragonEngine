#include <Engine/Core/ApplicationCore.h>
#include <Engine/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Engine
{
	class EngineEditor : public Application
	{
	public:
		EngineEditor() : Application("Dragon Engine Editor")
		{
			PushLayer(new EditorLayer());
		}
	};

	Application* CreateApplication()
	{
		return new EngineEditor();
	}
}
