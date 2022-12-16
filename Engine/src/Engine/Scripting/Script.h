#pragma once

#include "Engine/Scene/Entity.h"
#include "ScriptField.h"

#include <vector>

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
		friend class Scene;
		friend class EditorScene;

	private:
		void Clear();

	private:
		Entity AttachedEntity = { };

		MonoObject* ScriptObject = nullptr;
		uint32_t GCHandle = 0;

		MonoMethod* AttachToEntityMethod = nullptr;
		MonoMethod* BeginPlayMethod = nullptr;
		MonoMethod* UpdateMethod = nullptr;
	};

	class ScriptData
	{
	public:
		const std::string& GetName() const { return Name; }
		const std::string& GetNameSpace() const { return NameSpace; }
		const MonoClass* GetClass() const { return Class; }
		std::vector<ScriptField>& GetFields() { return Fields; }

		bool IsSame(const ScriptData& Data) const
		{
			return (Data.Name == Name) && (Data.NameSpace == NameSpace);
		}

		friend class ScriptEngine;

	private:
		ScriptData(const std::string& Name, const std::string& NameSpace, MonoClass* Class)
			: Name(Name), NameSpace(NameSpace), Class(Class) { }

	private:
		std::string Name;
		std::string NameSpace;

		MonoClass* Class = nullptr;

		std::vector<ScriptField> Fields = { };
	};
}