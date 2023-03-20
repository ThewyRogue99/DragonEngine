#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

extern "C" {
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoObject MonoObject;
}

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
		const std::string& GetName() const { return Name; }
		const ScriptFieldType GetFieldType() const { return FieldType; }
		const ScriptFieldVisibility GetFieldVisibility() const { return FieldVisibility; }

		size_t GetFieldSize() const;

		bool IsSameField(const ScriptFieldInfo& field);

		friend class ScriptField;
		friend class ScriptEngine;

	private:
		ScriptFieldInfo(MonoClassField* Field);

	private:
		MonoClassField* ClassField = nullptr;

		std::string Name;

		ScriptFieldType FieldType = ScriptFieldType::None;
		ScriptFieldVisibility FieldVisibility = ScriptFieldVisibility::Hidden;
	};

	class ScriptField
	{
	public:
		bool IsValid() const { return ScriptObject && Info; }

		const ScriptFieldInfo* GetInfo() const { return Info; }

		void SetValue(void* value) const;

		void GetValue(void* value) const;

		template<typename T>
		void Set(T* value) const;

		template<typename T>
		void Get(T* value) const;

		friend class Script;

	private:
		ScriptField() : ScriptField(nullptr, nullptr) { }

		ScriptField(MonoObject* ScriptObject, const ScriptFieldInfo* Info)
			: ScriptObject(ScriptObject), Info(Info) { }

	private:
		MonoObject* ScriptObject;

		const ScriptFieldInfo* Info ;
	};
}