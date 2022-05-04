#include <Engine/Core/ApplicationCore.h>
#include <Engine/Core/EntryPoint.h>

//#define USE_SANDBOX_EXAMPLE
#define USE_SANDBOX_2D

#ifdef USE_SANDBOX_EXAMPLE
	#include "SandboxLayer.hpp"
#elif defined(USE_SANDBOX_2D)
	#include "Sandbox2D.h"
#endif

class Sandbox : public Engine::Application
{
public:
	Sandbox() : Engine::Application()
	{
		#ifdef USE_SANDBOX_EXAMPLE
			PushLayer(new SandboxLayer());
		#elif defined(USE_SANDBOX_2D)
			PushLayer(new Sandbox2D());
		#endif
	}
};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox();
}