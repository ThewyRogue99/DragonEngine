#pragma once

#ifdef ENGINE_PLATFORM_WINDOWS

extern Engine::Application* Engine::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	Engine::Log::Init();

	DE_PROFILE_BEGIN_SESSION("Startup", "EngineProfile-Startup.json");
	auto App = Engine::CreateApplication({ argc, argv });
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