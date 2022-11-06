#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Debug/Instrumentor.h"

#ifdef ENGINE_PLATFORM_WINDOWS

#include <Windows.h>

extern Engine::Application* Engine::CreateApplication(ApplicationCommandLineArgs args);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	DE_PROFILE_BEGIN_SESSION("Startup", "EngineProfile-Startup.json");
	auto App = Engine::CreateApplication({ __argc, __argv });
	DE_PROFILE_END_SESSION();

	DE_PROFILE_BEGIN_SESSION("Runtime", "EngineProfile-Runtime.json");
	App->Run();
	DE_PROFILE_END_SESSION();

	DE_PROFILE_BEGIN_SESSION("Shutdown", "EngineProfile-Shutdown.json");
	delete App;
	DE_PROFILE_END_SESSION();

	return 1;
}

int main()
{
	DE_PROFILE_BEGIN_SESSION("Startup", "EngineProfile-Startup.json");
	auto App = Engine::CreateApplication({ __argc, __argv });
	DE_PROFILE_END_SESSION();

	DE_PROFILE_BEGIN_SESSION("Runtime", "EngineProfile-Runtime.json");
	App->Run();
	DE_PROFILE_END_SESSION();

	DE_PROFILE_BEGIN_SESSION("Shutdown", "EngineProfile-Shutdown.json");
	delete App;
	DE_PROFILE_END_SESSION();

	return 1;
}

#else
	#error Dragon Engine only supports Windows
#endif