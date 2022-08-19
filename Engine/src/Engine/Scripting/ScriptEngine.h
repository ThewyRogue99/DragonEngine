#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "Script.h"

namespace Engine
{
	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void Run();
		static void Stop();

		static void LoadAllScripts();

		static Script* NewScript(const std::string& ScriptNamespace, const std::string& ScriptName);

		static void Update(float DeltaTime);

		static void* GetCoreAssemblyImage();

	private:
		static void MonoInit();
	};
}