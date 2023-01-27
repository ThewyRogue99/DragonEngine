#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "Script.h"

#include <vector>

namespace Engine
{
	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void SetAssemblyPath(const CString& CoreAssemblyPath, const CString& AppAssemblyPath);

		static void Run();
		static void Stop();

		static bool LoadCore();
		static bool LoadApp(bool Reload = false);

		static bool Load(bool Reload = false);

		static void LoadAllScripts(bool Reload = false);

		static ScriptData* GetScriptData(const std::string& ScriptNamespace, const std::string& ScriptName);
		static Script* NewScript(const std::string& ScriptNamespace, const std::string& ScriptName);

		static bool ScriptExists(const std::string& ScriptNamespace, const std::string& ScriptName);

		static std::vector<ScriptData>& GetScriptDataList();

		static void Update(float DeltaTime);

		static void* GetCoreAssemblyImage();

	private:
		static void MonoInit();

		static void GetFields(ScriptData& data);
	};
}