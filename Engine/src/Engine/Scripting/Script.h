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
	class ScriptInfo;

	class Script
	{
	public:
		~Script();

		void AttachToEntity(Entity entity);

		ScriptField GetField(const std::string& FieldName) const;
		const std::vector<ScriptField>& GetFields() const { return Fields; }

		MonoObject* GetScriptObject() const { return ScriptObject; }

		Script(const Script&) = delete;
		const Script& operator =(const Script&) = delete;

		friend class ScriptEngine;

	private:
		struct phold {
			explicit phold(int) {}
		};

	public:
		explicit Script(const phold&, MonoObject* ScriptObject, uint32_t GCHandle, ScriptInfo* Info)
			: Script(ScriptObject, GCHandle, Info) { }

	private:
		Script(MonoObject* ScriptObject, uint32_t GCHandle, ScriptInfo* Info)
			: ScriptObject(ScriptObject), GCHandle(GCHandle), Info(Info) { GenerateFields(); }

		void GenerateFields();

		bool BeginPlay();

		bool Update(float DeltaTime);

		void Clear();

	private:
		Entity AttachedEntity = { };

		MonoObject* ScriptObject = nullptr;
		uint32_t GCHandle = 0;

		ScriptInfo* Info = nullptr;

		std::vector<ScriptField> Fields = { };

		MonoMethod* AttachToEntityMethod = nullptr;
		MonoMethod* BeginPlayMethod = nullptr;
		MonoMethod* UpdateMethod = nullptr;
	};

	class ScriptInfo
	{
	public:
		const std::string& GetName() const { return Name; }
		const std::string& GetNameSpace() const { return NameSpace; }
		const MonoClass* GetClass() const { return Class; }

		const std::vector<ScriptFieldInfo>& GetScriptFieldInfos() const
			{ return ScriptFieldInfoList; }

		bool IsSame(const ScriptInfo& Data) const
		{
			return (Data.Name == Name) && (Data.NameSpace == NameSpace);
		}

		friend class ScriptEngine;

	private:
		ScriptInfo(const std::string& NameSpace,const std::string& Name, MonoClass* Class)
			: NameSpace(NameSpace), Name(Name), Class(Class) { }

	private:
		std::string Name;
		std::string NameSpace;

		MonoClass* Class = nullptr;

		std::vector<ScriptFieldInfo> ScriptFieldInfoList = { };
	};
}