#include <Engine/Core/ApplicationCore.h>
#include <Engine/Core/EntryPoint.h>

#include "Sandbox/Sandbox2D.h"

class Sandbox : public Engine::Application
{
public:
	Sandbox(Engine::ApplicationCommandLineArgs args) : Engine::Application("Sandbox", args)
	{
		PushLayer(new Sandbox2D());
	}
};

Engine::Application* Engine::CreateApplication(Engine::ApplicationCommandLineArgs args)
{
	return new Sandbox(args);
}