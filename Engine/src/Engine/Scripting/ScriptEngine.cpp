#include "depch.h"

#include "ScriptEngine.h"
#include "ScriptInternals.h"

#include "Engine/Core/Log.h"
#include "ScriptField.h"

#include <algorithm>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/attrdefs.h>

#if defined(ENGINE_BUILD_DEBUG) | defined(ENGINE_BUILD_RELEASE)
	#define MONO_PATH "../vendor/lib"
#else
	#define MONO_PATH "lib"
#endif

namespace Engine
{
	static bool bShouldRun = false;

	static MonoDomain* RootDomain = nullptr;
	static MonoDomain* AppDomain = nullptr;

	struct AssemblyInfo
	{
		CString Path = L"";

		MonoAssembly* Assembly = nullptr;
		MonoImage* Image = nullptr;
	};

	static AssemblyInfo CoreAssembly;
	static AssemblyInfo AppAssembly;

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
			MonoImage* image = mono_image_open_from_data_full((char*)data.Data, (uint32_t)data.size, true, &status, false);

			std::string path_str = TypeUtils::FromUTF16(AssemblyFilePath);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				DE_ERROR(ScriptEngine, errorMessage);

				delete[] data.Data;
				return nullptr;
			}
			else if (!image)
			{
				DE_ERROR(ScriptEngine, "Failed to load assembly in path: {0}", path_str.c_str());

				delete[] data.Data;
				return nullptr;
			}

			MonoAssembly* assembly = mono_assembly_load_from_full(image, path_str.c_str(), &status, false);
			mono_image_close(image);

			delete[] data.Data;

			return assembly;
		}
	}

	void ScriptEngine::Init()
	{
		MonoInit();

		DE_INFO(ScriptEngine, "Initialized ScriptEngine");
	}

	void ScriptEngine::Shutdown()
	{
		Stop();

		mono_jit_cleanup(RootDomain);

		ScriptDataList.clear();

		DE_WARN(ScriptEngine, "Shutting down ScriptEngine");
	}

	void ScriptEngine::SetAssemblyPath(const CString& CoreAssemblyPath, const CString& AppAssemblyPath)
	{
		CoreAssembly.Path = CoreAssemblyPath;
		AppAssembly.Path = AppAssemblyPath;

		DE_WARN(ScriptEngine, "Setting new assembly paths for ScriptEngine");
	}

	void ScriptEngine::Run()
	{
		bShouldRun = true;

		DE_INFO(ScriptEngine, "Running ScriptEngine");

		for (auto& [name, script] : ScriptObjectList)
		{
			if(script)
				script->BeginPlay();
		}
	}

	void ScriptEngine::Stop()
	{
		bShouldRun = false;

		DE_WARN(ScriptEngine, "Stopping ScriptEngine");

		for (auto& [name, ptr] : ScriptObjectList)
			delete ptr;

		ScriptObjectList.clear();
	}

	bool ScriptEngine::LoadCore()
	{
		DE_LOG(ScriptEngine, "Loading Core Assembly...");

		CoreAssembly.Assembly = Utils::LoadAssembly(CoreAssembly.Path);

		if (CoreAssembly.Assembly)
		{
			CoreAssembly.Image = mono_assembly_get_image(CoreAssembly.Assembly);

			ScriptInternals::RegisterFunctions();
			ScriptInternals::RegisterComponents();

			DE_INFO(ScriptEngine, "Successfully loaded Core Assembly");

			return true;
		}

		DE_ERROR(ScriptEngine, "Failed to load Core Assembly");
		return false;
	}

	bool ScriptEngine::LoadApp(bool Reload)
	{
		DE_LOG(ScriptEngine, "Loading App Assembly...");

		AppAssembly.Assembly = Utils::LoadAssembly(AppAssembly.Path);

		if (AppAssembly.Assembly)
		{
			AppAssembly.Image = mono_assembly_get_image(AppAssembly.Assembly);

			ScriptEngine::LoadAllScripts(Reload);

			DE_INFO(ScriptEngine, "Successfully loaded App Assembly");

			return true;
		}

		DE_ERROR(ScriptEngine, "Failed to load App Assembly");
		return false;
	}

	bool ScriptEngine::Load(bool Reload)
	{
		if (AppDomain)
		{
			mono_domain_set(mono_get_root_domain(), false);
			mono_domain_unload(AppDomain);
		}

		AppDomain = mono_domain_create_appdomain("DEScript_AppDomain", nullptr);
		mono_domain_set(AppDomain, true);

		return LoadCore() && LoadApp(Reload);
	}

	void ScriptEngine::GetFields(ScriptData& data)
	{
		// Get Fields
		int fieldCount = mono_class_num_fields(data.Class);
		MonoClassField* iterator = nullptr;

		for (int i = 0; i < fieldCount; i++)
		{
			mono_class_get_fields(data.Class, (void**)&iterator);

			ScriptFieldVisibility visibility = ScriptFieldVisibility::Hidden;

			if ((mono_field_get_flags(iterator) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK) == MONO_FIELD_ATTR_PUBLIC)
				visibility = ScriptFieldVisibility::Visible;

			MonoCustomAttrInfo* inf = mono_custom_attrs_from_field(data.Class, iterator);
			if (inf)
			{
				MonoArray* attrArray = mono_custom_attrs_construct(inf);

				uintptr_t len = mono_array_length(attrArray);

				for (uintptr_t i = 0; i < len; i++)
				{
					MonoObject* obj = mono_array_get(attrArray, MonoObject*, i);
					MonoClass* attrClass = mono_object_get_class(obj);
					MonoType* attrType = mono_class_get_type(attrClass);

					if (strcmp("DragonEngine.EditorReflection", mono_type_get_name(attrType)) == 0)
					{
						MonoClassField* VisibilityField = mono_class_get_field_from_name(attrClass, "_visibility");

						mono_field_get_value(obj, VisibilityField, &visibility);
					}

					mono_custom_attrs_free(inf);
				}
			}

			if (visibility != ScriptFieldVisibility::Hidden)
			{
				ScriptField f(iterator);
				f.FieldVisibility = visibility;

				auto& it = std::find_if(data.Fields.begin(), data.Fields.end(), [&f](const ScriptField& field)
				{
					return f.GetName() == field.GetName();
				});

				if (it != data.Fields.end())
				{
					(*it).FieldVisibility = visibility;
					(*it).ClassField = iterator;
				}
				else
				{
					data.Fields.push_back(f);
				}
			}
		}
	}

	void ScriptEngine::LoadAllScripts(bool Reload)
	{
		MonoImage* CoreImage = CoreAssembly.Image;
		MonoImage* AppImage = AppAssembly.Image;

		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(AppImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* scriptBaseClass = mono_class_from_name(CoreImage, "DragonEngine", "Script");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(AppImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(AppImage, cols[MONO_TYPEDEF_NAME]);

			MonoClass* scriptClass = mono_class_from_name(AppImage, nameSpace, name);

			if (scriptClass == scriptBaseClass)
				continue;

			if (scriptClass)
			{
				bool isScript = mono_class_is_subclass_of(scriptClass, scriptBaseClass, false);
				if (isScript)
				{
					ScriptData data(name, nameSpace, scriptClass);
					
					if (Reload)
					{
						auto& it = std::find_if(ScriptDataList.begin(), ScriptDataList.end(), [data](const ScriptData& d)
						{
							return data.IsSame(d);
						});

						if (it != ScriptDataList.end())
						{
							(*it).Class = scriptClass;
							GetFields(*it);
						}
					}
					else
					{
						GetFields(data);
						ScriptDataList.push_back(data);

						DE_LOG(ScriptEngine, "Loading Script: {0}.{1}", nameSpace, name);
					}
				}
			}
		}
	}

	ScriptData* ScriptEngine::GetScriptData(const std::string& ScriptNamespace, const std::string& ScriptName)
	{
		if (!ScriptName.empty())
		{
			auto it = std::find_if(ScriptDataList.begin(), ScriptDataList.end(), [ScriptNamespace, ScriptName](const ScriptData& data)
			{
				return (data.Name == ScriptName) && (data.NameSpace == ScriptNamespace);
			});

			if (it != ScriptDataList.end())
			{
				return &(*it);
			}
		}

		return nullptr;
	}

	Script* ScriptEngine::NewScript(const std::string& ScriptNamespace, const std::string& ScriptName)
	{
		const ScriptData* data = GetScriptData(ScriptNamespace, ScriptName);

		if (data)
		{
			MonoObject* ScriptObject = mono_object_new(AppDomain, data->Class);
			if (ScriptObject)
			{
				mono_runtime_object_init(ScriptObject);

				Script* newScript = new Script();
				newScript->GCHandle = mono_gchandle_new(ScriptObject, false);

				MonoClass* scriptBaseClass = mono_class_from_name(CoreAssembly.Image, "DragonEngine", "Script");

				MonoMethod* vMethod = mono_class_get_method_from_name(scriptBaseClass, "AttachToEntity", 1);
				newScript->AttachToEntityMethod = mono_object_get_virtual_method(ScriptObject, vMethod);

				newScript->BeginPlayMethod = mono_class_get_method_from_name(data->Class, "BeginPlay", 0);
				newScript->UpdateMethod = mono_class_get_method_from_name(data->Class, "Update", 1);

				newScript->ScriptObject = ScriptObject;

				DE_INFO("Creating Script: {0}.{1}", ScriptNamespace.c_str(), ScriptName.c_str());

				ScriptObjectList[ScriptName] = newScript;

				return newScript;
			}
		}


		DE_INFO("Failed to find Script: {0}.{1}", ScriptNamespace.c_str(), ScriptName.c_str());
		return nullptr;
	}

	void ScriptEngine::DestroyScript(Script* ScriptToDestroy)
	{
		mono_gchandle_free(ScriptToDestroy->GCHandle);
		ScriptToDestroy->Clear();
	}

	bool ScriptEngine::ScriptExists(const std::string& ScriptNamespace, const std::string& ScriptName)
	{
		return GetScriptData(ScriptNamespace, ScriptName);
	}

	std::vector<ScriptData>& ScriptEngine::GetScriptDataList()
	{
		return ScriptDataList;
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
		return CoreAssembly.Image;
	}

	void ScriptEngine::MonoInit()
	{
		mono_set_dirs(MONO_PATH, MONO_PATH);

		RootDomain = mono_jit_init("DEScript_JITRuntime");

		DE_ASSERT(RootDomain, "Failed to initialize JIT");

		DE_LOG(ScriptEngine, "Initialized Mono");
	}
}