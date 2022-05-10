#include <Engine/Core/ApplicationCore.h>
#include <Engine/Core/EntryPoint.h>

#include "Sandbox/Sandbox2D.h"

class Sandbox : public Engine::Application
{
public:
	Sandbox() : Engine::Application()
	{
		PushLayer(new Sandbox2D());
	}
};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox();
}