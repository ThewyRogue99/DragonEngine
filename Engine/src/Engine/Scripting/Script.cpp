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
				uint64_t id = entity.GetUUID();

				void* idParam = &id;
				mono_runtime_invoke(AttachToEntityMethod, ScriptObject, &idParam, nullptr);
			}
		}
	}
}