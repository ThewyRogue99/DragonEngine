#include "depch.h"
#include "Script.h"

#include "Engine/Scene/Scene.h"

#include <mono/metadata/object.h>

namespace Engine
{
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
}