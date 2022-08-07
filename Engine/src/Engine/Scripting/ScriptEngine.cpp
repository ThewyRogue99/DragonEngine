#include "depch.h"
#include "ScriptEngine.h"

#include "ScriptInternals.h"

#include "Engine/Core/Log.h"

#include <vector>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Engine
{
	static class Script
	{
	public:
		void BeginPlay();

		void Update(float DeltaTime);

		friend class ScriptEngine;

	private:
		MonoObject* ScriptObject = nullptr;

		MonoMethod* BeginPlayMethod = nullptr;
		MonoMethod* UpdateMethod = nullptr;
	};

	static MonoDomain* RootDomain = nullptr;
	static MonoDomain* AppDomain = nullptr;

	static MonoAssembly* CoreAssembly = nullptr;

	static MonoImage* AssemblyImage = nullptr;

	static std::vector<Script> ScriptList = { };

	void ScriptEngine::Init()
	{
		MonoInit();

		CoreAssembly = LoadAssembly(TEXT("../bin/Resources/Scripts/DE_ScriptCore.dll"));

		AssemblyImage = mono_assembly_get_image(CoreAssembly);

		ScriptInternals::RegisterFunctions();

		LoadScript("MainScript");
	}

	void ScriptEngine::Shutdown()
	{
		mono_jit_cleanup(RootDomain);
	}

	bool ScriptEngine::LoadScript(const char* ScriptName)
	{
		if (AssemblyImage)
		{
			MonoClass* scriptClass = mono_class_from_name(AssemblyImage, "EngineScript", ScriptName);
			if (scriptClass)
			{
				MonoObject* scriptObject = mono_object_new(AppDomain, scriptClass);
				mono_runtime_object_init(scriptObject);

				Script newScript;
				newScript.ScriptObject = scriptObject;
				newScript.BeginPlayMethod = mono_class_get_method_from_name(scriptClass, "BeginPlay", 0);
				newScript.UpdateMethod = mono_class_get_method_from_name(scriptClass, "Update", 1);

				newScript.BeginPlay();
				ScriptList.push_back(newScript);
			}
		}

		return false;
	}

	void ScriptEngine::Update(float DeltaTime)
	{
		for (auto& script : ScriptList)
			script.Update(DeltaTime);
	}

	void ScriptEngine::MonoInit()
	{
		RootDomain = mono_jit_init("DEScript_JITRuntime");

		DE_CORE_ASSERT(RootDomain, "Failed to initialize JIT");

		AppDomain = mono_domain_create_appdomain("DEScript_AppDomain", nullptr);
		DE_CORE_ASSERT(AppDomain, "Failed to create an app domain");

		mono_domain_set(AppDomain, true);
	}

	MonoAssembly* ScriptEngine::LoadAssembly(const CString& AssemblyFilePath)
	{
		FileData data = GetAssemblyFileBytes(AssemblyFilePath);

		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full((char*)data.Data, data.size, true, &status, false);

		std::string path_str = TypeUtils::FromUTF16(AssemblyFilePath);

		if (status != MONO_IMAGE_OK)
		{
			const char* errorMessage = mono_image_strerror(status);
			DE_CORE_CRITICAL(errorMessage);

			delete[] data.Data;
			return nullptr;
		}
		else if (!image)
		{
			DE_CORE_CRITICAL("Failed to load assmebly in path: {0}", path_str.c_str());

			delete[] data.Data;
			return nullptr;
		}

		MonoAssembly* assembly = mono_assembly_load_from_full(image, path_str.c_str(), &status, false);
		mono_image_close(image);

		delete[] data.Data;

		return assembly;
	}

	ScriptEngine::FileData ScriptEngine::GetAssemblyFileBytes(const CString& AssemblyFilePath)
	{
		std::ifstream stream(AssemblyFilePath, std::ios::binary | std::ios::ate);

		if (stream)
		{
			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			size_t size = end - stream.tellg();

			if (size > 0)
			{
				uint8_t* buffer = new uint8_t[size];
				stream.read((char*)buffer, size);
				stream.close();

				return { buffer, size };
			}
		}

		return FileData();
	}

	void Script::BeginPlay()
	{
		if (ScriptObject && BeginPlayMethod)
		{
			mono_runtime_invoke(BeginPlayMethod, ScriptObject, nullptr, nullptr);
		}
	}

	void Script::Update(float DeltaTime)
	{
		if (ScriptObject && UpdateMethod)
		{
			void* timeParam = &DeltaTime;
			mono_runtime_invoke(UpdateMethod, ScriptObject, &timeParam, nullptr);
		}
	}
}