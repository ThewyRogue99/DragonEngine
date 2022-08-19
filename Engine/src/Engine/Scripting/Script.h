#pragma once

#include "Engine/Scene/Entity.h"

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
}

namespace Engine
{
	class Script
	{
	public:
		void BeginPlay();

		void Update(float DeltaTime);

		void AttachToEntity(Entity entity);

		friend class ScriptEngine;

		std::string Name = "";
		std::string Namespace = "";

	private:
		Entity AttachedEntity = { };

		MonoObject* ScriptObject = nullptr;

		MonoMethod* AttachToEntityMethod = nullptr;
		MonoMethod* BeginPlayMethod = nullptr;
		MonoMethod* UpdateMethod = nullptr;
	};
}