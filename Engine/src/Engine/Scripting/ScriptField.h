#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

extern "C" {
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoObject MonoObject;
}

#define DECLARE_SCRIPT_FIELD_TYPE(Type) \
template<> \
void ScriptField::Set<Type>(Type* value) const;

namespace Engine
{
	class Script;

	enum class ScriptFieldType
	{
		None = 0,
		Float, Double,
		Bool, Char, Short, Int, Long,
		Byte, UShort, UInt, ULong,
		Vector2, Vector3, Vector4,
		Entity
	};

	enum class ScriptFieldVisibility
	{
		Visible,
		Hidden
	};

	class ScriptFieldInfo
	{
	public:
		const CString& GetName() const { return Name; }
		const ScriptFieldType GetFieldType() const { return FieldType; }
		const ScriptFieldVisibility GetFieldVisibility() const { return FieldVisibility; }

		ENGINE_API size_t GetFieldSize() const;

		ENGINE_API bool IsSameField(const ScriptFieldInfo& field);

		friend class ScriptField;
		friend class ScriptEngine;

	private:
		ENGINE_API ScriptFieldInfo(MonoClassField* Field);

	private:
		MonoClassField* ClassField = nullptr;

		CString Name;

		ScriptFieldType FieldType = ScriptFieldType::None;
		ScriptFieldVisibility FieldVisibility = ScriptFieldVisibility::Hidden;
	};

	class ScriptField
	{
	public:
		bool IsValid() const { return ScriptObject && Info; }

		const ScriptFieldInfo* GetInfo() const { return Info; }

		ENGINE_API void SetValue(void* value) const;

		ENGINE_API void GetValue(void* value) const;

		// TODO: Implement Entity type field set/get

		template<typename T>
		void Set(T* value) const
		{
			if (value)
			{
				SetValue((void*)value);
			}
		}

		template<typename T>
		void Get(T* value) const
		{
			if (value)
			{
				GetValue((void*)value);
			}
		}

		friend class Script;

	private:
		ScriptField() : ScriptField(nullptr, nullptr) { }

		ScriptField(MonoObject* ScriptObject, const ScriptFieldInfo* Info)
			: ScriptObject(ScriptObject), Info(Info) { }

	private:
		MonoObject* ScriptObject;

		const ScriptFieldInfo* Info;
	};
}