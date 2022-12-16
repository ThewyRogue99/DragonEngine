#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include <typeindex>

extern "C" {
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoObject MonoObject;
}

namespace Engine
{
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

	class ScriptField
	{
	public:
		ScriptField(MonoClassField* Field);

		const std::string& GetName() const { return Name; }
		const ScriptFieldType GetFieldType() const { return FieldType; }
		const ScriptFieldVisibility GetFieldVisibility() const { return FieldVisibility; }

		void Set(MonoObject* Object);

		template<typename T>
		void SetValue(MonoObject* Object, T* value);

		template<typename T>
		void SetBufferValue(T* value)
		{
			if (FieldBuffer)
				delete (T*)FieldBuffer;

			BufferSize = sizeof(T);
			FieldBuffer = new T(*value);
		}

		void Get(MonoObject* Object);

		template<typename T>
		const T& GetValue(MonoObject* Object);

		template<typename T>
		const T& GetBufferValue()
		{
			if (!FieldBuffer)
			{
				FieldBuffer = new T();
				BufferSize = sizeof(T);
			}

			return *((T*)FieldBuffer);
		}

		void* GetBufferData() const { return FieldBuffer; }
		size_t GetBufferSize() const { return BufferSize; }

		void SetBufferData(void* Data, size_t size);

		bool IsSameField(const ScriptField& field);

		friend class ScriptEngine;

	private:
		MonoClassField* ClassField = nullptr;

		std::string Name;

		void* FieldBuffer = nullptr;
		size_t BufferSize = 0;

		ScriptFieldType FieldType = ScriptFieldType::None;
		ScriptFieldVisibility FieldVisibility = ScriptFieldVisibility::Hidden;
	};
}