#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

extern "C" {
	typedef struct _MonoAssembly MonoAssembly;
}

namespace Engine
{
	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static bool LoadScript(const char* ScriptName);

		static void Update(float DeltaTime);

	private:
		static void MonoInit();

		struct FileData
		{
			uint8_t* Data = nullptr;
			size_t size = 0;
		};

		static MonoAssembly* LoadAssembly(const CString& AssemblyFilePath);

		static FileData GetAssemblyFileBytes(const CString& AssemblyFilePath);
	};
}