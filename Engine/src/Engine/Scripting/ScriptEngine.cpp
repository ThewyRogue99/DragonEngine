#include "depch.h"

#include "ScriptEngine.h"
#include "ScriptInternals.h"

#include "Engine/Core/Log.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Engine
{
	static bool bShouldRun = false;

	static MonoDomain* RootDomain = nullptr;
	static MonoDomain* AppDomain = nullptr;

	static MonoAssembly* CoreAssembly = nullptr;

	static MonoImage* AssemblyImage = nullptr;

	struct ScriptData;

	static std::unordered_map<std::string, Script*> ScriptObjectList = { };
	static std::vector<ScriptData> ScriptDataList = { };

	namespace Utils
	{
		struct FileData
		{
			uint8_t* Data = nullptr;
			size_t size = 0;
		};

		FileData GetAssemblyFileBytes(const CString& AssemblyFilePath)
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

		MonoAssembly* LoadAssembly(const CString& AssemblyFilePath)
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
	}

	struct ScriptData
	{
		std::string Name = "";
		std::string NameSpace = "";

		MonoClass* Class = nullptr;
	};

	void ScriptEngine::Init()
	{
		MonoInit();

		CoreAssembly = Utils::LoadAssembly(TEXT("../bin/Resources/Scripts/DE_ScriptCore.dll"));

		AssemblyImage = mono_assembly_get_image(CoreAssembly);

		ScriptInternals::RegisterFunctions();
		ScriptInternals::RegisterComponents();

		ScriptEngine::LoadAllScripts();
	}

	void ScriptEngine::Shutdown()
	{
		Stop();

		mono_jit_cleanup(RootDomain);

		ScriptDataList.clear();
	}

	void ScriptEngine::Run()
	{
		bShouldRun = true;

		for (auto& [name, script] : ScriptObjectList)
		{
			if(script)
				script->BeginPlay();
		}
	}

	void ScriptEngine::Stop()
	{
		bShouldRun = false;

		for (auto& [name, ptr] : ScriptObjectList)
			delete ptr;

		ScriptObjectList.clear();
	}

	void ScriptEngine::LoadAllScripts()
	{
		ScriptDataList.clear();

		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(AssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* scriptBaseClass = mono_class_from_name(AssemblyImage, "DragonEngine", "Script");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(AssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(AssemblyImage, cols[MONO_TYPEDEF_NAME]);

			MonoClass* scriptClass = mono_class_from_name(AssemblyImage, nameSpace, name);

			if (scriptClass == scriptBaseClass)
				continue;

			if (scriptClass)
			{
				bool isScript = mono_class_is_subclass_of(scriptClass, scriptBaseClass, false);
				if (isScript)
					ScriptDataList.push_back({ name, nameSpace, scriptClass });
			}
		}

		return;
	}

	Script* ScriptEngine::NewScript(const std::string& ScriptNamespace, const std::string& ScriptName)
	{
		auto it = std::find_if(ScriptDataList.begin(), ScriptDataList.end(), [ScriptNamespace, ScriptName](const ScriptData& data)
		{
			return (data.Name == ScriptName) && (data.NameSpace == ScriptNamespace);
		});

		if (it != ScriptDataList.end())
		{
			ScriptData& data = *it;

			MonoObject* ScriptObject = mono_object_new(AppDomain, data.Class);
			if (ScriptObject)
			{
				mono_runtime_object_init(ScriptObject);

				MonoClass* scriptBaseClass = mono_class_from_name(AssemblyImage, "DragonEngine", "Script");

				Script* newScript = new Script();

				MonoMethod* vMethod = mono_class_get_method_from_name(scriptBaseClass, "AttachToEntity", 1);
				newScript->AttachToEntityMethod = mono_object_get_virtual_method(ScriptObject, vMethod);

				newScript->BeginPlayMethod = mono_class_get_method_from_name(data.Class, "BeginPlay", 0);
				newScript->UpdateMethod = mono_class_get_method_from_name(data.Class, "Update", 1);

				newScript->ScriptObject = ScriptObject;

				ScriptObjectList[ScriptName] = newScript;

				return newScript;
			}
		}

		return nullptr;
	}

	bool ScriptEngine::ScriptExists(const std::string& ScriptNamespace, const std::string& ScriptName)
	{
		auto it = std::find_if(ScriptDataList.begin(), ScriptDataList.end(), [ScriptNamespace, ScriptName](const ScriptData& data)
		{
			return (data.Name == ScriptName) && (data.NameSpace == ScriptNamespace);
		});

		return it != ScriptDataList.end();
	}

	std::vector<std::pair<std::string, std::string>> ScriptEngine::GetScriptData()
	{
		size_t size = ScriptDataList.size();
		std::vector<std::pair<std::string, std::string>> data(size);

		for (size_t i = 0; i < size; i++)
		{
			data[i] = { ScriptDataList[i].Name, ScriptDataList[i].NameSpace };
		}

		return data;
	}

	void ScriptEngine::Update(float DeltaTime)
	{
		if (bShouldRun)
		{
			for (auto& [name, script] : ScriptObjectList)
			{
				if(script)
					script->Update(DeltaTime);
			}
		}
	}

	void* ScriptEngine::GetCoreAssemblyImage()
	{
		return AssemblyImage;
	}

	void ScriptEngine::MonoInit()
	{
		RootDomain = mono_jit_init("DEScript_JITRuntime");

		DE_CORE_ASSERT(RootDomain, "Failed to initialize JIT");

		AppDomain = mono_domain_create_appdomain("DEScript_AppDomain", nullptr);
		DE_CORE_ASSERT(AppDomain, "Failed to create an app domain");

		mono_domain_set(AppDomain, true);
	}
}