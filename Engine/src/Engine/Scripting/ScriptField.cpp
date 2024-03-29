#include "depch.h"
#include "ScriptField.h"

#include "Engine/Debug/Log.h"

#include "Script.h"

#include <mono/metadata/object.h>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Engine
{
	static std::unordered_map<CString, ScriptFieldType> s_ScriptFieldTypeMap =
	{
		{ "System.Single", ScriptFieldType::Float },
		{ "System.Double", ScriptFieldType::Double },
		{ "System.Boolean", ScriptFieldType::Bool },
		{ "System.Char", ScriptFieldType::Char },
		{ "System.Int16", ScriptFieldType::Short },
		{ "System.Int32", ScriptFieldType::Int },
		{ "System.Int64", ScriptFieldType::Long },
		{ "System.Byte", ScriptFieldType::Byte },
		{ "System.UInt16", ScriptFieldType::UShort },
		{ "System.UInt32", ScriptFieldType::UInt },
		{ "System.UInt64", ScriptFieldType::ULong },

		{ "DragonEngine.Vector2", ScriptFieldType::Vector2 },
		{ "DragonEngine.Vector3", ScriptFieldType::Vector3 },
		{ "DragonEngine.Vector4", ScriptFieldType::Vector4 },

		{ "DragonEngine.Entity", ScriptFieldType::Entity},
	};

	static ScriptFieldType MonoTypeToScriptFieldData(MonoType* monoType)
	{
		CString typeName = mono_type_get_name(monoType);

		auto it = s_ScriptFieldTypeMap.find(typeName);
		if (it == s_ScriptFieldTypeMap.end())
		{
			DE_ERROR(ScriptFieldType, "Unknown type: {0}", typeName);
			return ScriptFieldType::None;
		}

		return it->second;
	}

	ScriptFieldInfo::ScriptFieldInfo(MonoClassField* Field)
	{
		Name = mono_field_get_name(Field);
		ClassField = Field;
		FieldType = MonoTypeToScriptFieldData(mono_field_get_type(Field));
	}

	size_t ScriptFieldInfo::GetFieldSize() const
	{
		switch (FieldType)
		{
		case Engine::ScriptFieldType::Float:
			return sizeof(float);
		case Engine::ScriptFieldType::Double:
			return sizeof(double);
		case Engine::ScriptFieldType::Bool:
			return sizeof(bool);
		case Engine::ScriptFieldType::Char:
			return sizeof(char);
		case Engine::ScriptFieldType::Short:
			return sizeof(short);
		case Engine::ScriptFieldType::Int:
			return sizeof(int);
		case Engine::ScriptFieldType::Long:
			return sizeof(long);
		case Engine::ScriptFieldType::Byte:
			return sizeof(unsigned char);
		case Engine::ScriptFieldType::UShort:
			return sizeof(unsigned short);
		case Engine::ScriptFieldType::UInt:
			return sizeof(unsigned int);
		case Engine::ScriptFieldType::ULong:
			return sizeof(unsigned long);
		case Engine::ScriptFieldType::Vector2:
			return sizeof(glm::vec2);
		case Engine::ScriptFieldType::Vector3:
			return sizeof(glm::vec3);
		case Engine::ScriptFieldType::Vector4:
			return sizeof(glm::vec4);
		case Engine::ScriptFieldType::Entity:
			return 0;
		default:
			return 0;
		}
	}

	bool ScriptFieldInfo::IsSameField(const ScriptFieldInfo& field)
	{
		if (mono_field_get_parent(ClassField) == mono_field_get_parent(field.ClassField))
		{
			if (Name == field.Name)
				return true;
		}

		return false;
	}

	void ScriptField::SetValue(void* value) const
	{
		mono_field_set_value(ScriptObject, Info->ClassField, (void*)value);
	}

	void ScriptField::GetValue(void* value) const
	{
		mono_field_get_value(ScriptObject, Info->ClassField, (void*)value);
	}
}