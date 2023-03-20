#include "depch.h"
#include "Script.h"

#include "Engine/Scene/Scene.h"

#include <mono/metadata/object.h>

namespace Engine
{
	Script::~Script()
	{
		mono_gchandle_free(GCHandle);
		Clear();
	}

	void Script::GenerateFields()
	{
		const std::vector<ScriptFieldInfo>& FieldInfos = Info->GetScriptFieldInfos();

		Fields.clear();

		for (int i = 0; i < FieldInfos.size(); i++)
		{
			Fields.push_back(ScriptField(ScriptObject, &(FieldInfos[i])));
		}
	}

	bool Script::BeginPlay()
	{
		if (ScriptObject && BeginPlayMethod)
		{
			MonoObject* exc = nullptr;
			mono_runtime_invoke(BeginPlayMethod, ScriptObject, nullptr, &exc);

			if (exc)
			{
				char* msg = mono_string_to_utf8(mono_object_to_string(exc, NULL));
				DE_ERROR(ScriptException, "Unhandled Exception: {0}", msg);
				mono_free(msg);

				return false;
			}

			return true;
		}

		return false;
	}

	bool Script::Update(float DeltaTime)
	{
		if (ScriptObject && UpdateMethod)
		{
			MonoObject* exc = nullptr;

			void* timeParam = &DeltaTime;
			mono_runtime_invoke(UpdateMethod, ScriptObject, &timeParam, &exc);

			if (exc)
			{
				char* msg = mono_string_to_utf8(mono_object_to_string(exc, NULL));
				DE_ERROR(ScriptException, "Unhandled Exception: {0}", msg);
				mono_free(msg);

				return false;
			}

			return true;
		}

		return false;
	}

	void Script::AttachToEntity(Entity entity)
	{
		AttachedEntity = entity;

		if (ScriptObject && AttachToEntityMethod)
		{
			if (entity.HasComponent<IDComponent>())
			{
				std::string id = entity.GetUUID().GetString();

				CString wID = TypeUtils::FromUTF8(id);

				MonoString* str = mono_string_from_utf16((mono_unichar2*)wID.c_str());

				mono_runtime_invoke(AttachToEntityMethod, ScriptObject, (void**)&str, nullptr);
			}
		}
	}

	ScriptField Script::GetField(const std::string& FieldName) const
	{
		auto it = std::find_if(Fields.begin(), Fields.end(), [FieldName](const ScriptField& field)
		{
			return field.GetInfo()->GetName() == FieldName;
		});

		if (it != Fields.end())
		{
			return *it;
		}

		return ScriptField(nullptr, nullptr);
	}

	void Script::Clear()
	{
		AttachedEntity = { };

		ScriptObject = nullptr;
		GCHandle = 0;

		AttachToEntityMethod = nullptr;
		BeginPlayMethod = nullptr;
		UpdateMethod = nullptr;
	}
}