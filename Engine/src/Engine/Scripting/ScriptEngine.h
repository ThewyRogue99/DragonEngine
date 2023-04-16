#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "Script.h"

#include <vector>

namespace Engine
{
	class ENGINE_API ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void SetAssemblyPath(const WString& CoreAssemblyPath, const WString& AppAssemblyPath);

		static void Run();
		static void Stop();

		static bool LoadCore();
		static bool LoadApp(bool Reload = false);

		static bool Load(bool Reload = false);

		static void LoadAllScripts(bool Reload = false);

		static ScriptInfo* GetScriptInfo(const CString& ScriptNamespace, const CString& ScriptName);
		static Ref<Script> NewScript(const CString& ScriptNamespace, const CString& ScriptName);

		static bool ScriptExists(const CString& ScriptNamespace, const CString& ScriptName);

		static void GetScriptDefaultFields(const CString& ScriptNamespace, const CString& ScriptName, MemoryMap& Result);

		static const std::vector<ScriptInfo*>& GetScriptInfoList();

		static void Update(float DeltaTime);

		static void* GetCoreAssemblyImage();

	private:
		static void MonoInit();

		static void GetScriptFields(ScriptInfo* info);
	};
}