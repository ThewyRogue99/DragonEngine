#pragma once

#ifdef ENGINE_PLATFORM_WINDOWS

extern Engine::Application* Engine::CreateApplication();

int main(int argc, char** argv)
{
	Engine::Log::Init();

	DE_CORE_WARN("Initialized Log!");
	DE_INFO("Hello!");

	auto App = Engine::CreateApplication();
	App->Run();
	delete App;

	return 1;
}

#else
	#error Dragon Engine only supports Windows
#endif