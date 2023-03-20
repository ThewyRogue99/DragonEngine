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

		static ScriptInfo* GetScriptInfo(const std::string& ScriptNamespace, const std::string& ScriptName);
		static Ref<Script> NewScript(const std::string& ScriptNamespace, const std::string& ScriptName);

		static bool ScriptExists(const std::string& ScriptNamespace, const std::string& ScriptName);

		static void GetScriptDefaultFields(const std::string& ScriptNamespace, const std::string& ScriptName, MemoryMap& Result);

		static const std::vector<ScriptInfo*>& GetScriptInfoList();

		static void Update(float DeltaTime);

		static void* GetCoreAssemblyImage();

	private:
		static void MonoInit();

		static void GetScriptFields(ScriptInfo* info);
	};
}