#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

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
		void Set(MonoObject* Object, T* value);

		template<typename T>
		void SetBuffer(T* value)
		{
			if (FieldBuffer)
				delete (T*)FieldBuffer;

			BufferSize = sizeof(T);
			FieldBuffer = new T(*value);
		}

		template<typename T>
		const T& Get(MonoObject* Object);

		template<typename T>
		const T& GetBuffer()
		{
			if (!FieldBuffer)
				FieldBuffer = new T();

			return *((T*)FieldBuffer);
		}

		void* GetBufferData() const { return FieldBuffer; }
		size_t GetBufferSize() const { return BufferSize; }

		bool IsSameField(const ScriptField& field);

		friend class ScriptEngine;

	private:
		void* FieldBuffer = nullptr;
		size_t BufferSize = 0;

	private:
		MonoClassField* ClassField = nullptr;

		std::string Name;

		ScriptFieldType FieldType = ScriptFieldType::None;
		ScriptFieldVisibility FieldVisibility = ScriptFieldVisibility::Hidden;
	};
}