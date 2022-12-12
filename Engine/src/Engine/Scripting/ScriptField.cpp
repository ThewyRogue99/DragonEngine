#include "depch.h"
#include "ScriptField.h"

#include "Engine/Scene/Entity.h"

#include <mono/metadata/object.h>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Engine
{
	static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap =
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

		{ "DragonEngine.Entity", ScriptFieldType::Entity },
	};

	static ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType)
	{
		std::string typeName = mono_type_get_name(monoType);

		auto it = s_ScriptFieldTypeMap.find(typeName);
		if (it == s_ScriptFieldTypeMap.end())
		{
			DE_CORE_ERROR("Unknown type: {0}", typeName);
			return ScriptFieldType::None;
		}

		return it->second;
	}

	ScriptField::ScriptField(MonoClassField* Field)
	{
		Name = mono_field_get_name(Field);
		ClassField = Field;
		FieldType = MonoTypeToScriptFieldType(mono_field_get_type(Field));
	}

	void ScriptField::Set(MonoObject* Object)
	{
		mono_field_set_value(Object, ClassField, FieldBuffer);
	}

	bool ScriptField::IsSameField(const ScriptField& field)
	{
		if (mono_field_get_parent(ClassField) == mono_field_get_parent(field.ClassField))
		{
			if (Name == field.Name)
				return true;
		}

		return false;
	}

#define FIELD_TYPE_FUNCTIONS_DEFINITION(Type, ScriptFieldType) \
template<> \
void ScriptField::Set(MonoObject* Object, Type* value) \
{ \
	if (Object) \
	{ \
		DE_CORE_ASSERT(FieldType == ScriptFieldType, "Cannot set a ScriptField of another type"); \
\
		mono_field_set_value(Object, ClassField, (void*)value); \
		SetBuffer(value); \
	} \
} \
\
template<> \
const Type& ScriptField::Get(MonoObject* Object) \
{ \
	const Type& value = GetBuffer<Type>(); \
	if (Object) \
	{ \
		DE_CORE_ASSERT(FieldType == ScriptFieldType, "Cannot get a ScriptField of another type"); \
\
		mono_field_get_value(Object, ClassField, (void*)&value); \
	} \
	return value; \
\
}

	template<typename T>
	void ScriptField::Set(MonoObject* Object, T* value)
	{
		DE_CORE_ASSERT(false, "Cannot set undefined ScriptFieldType");
	}

	template<typename T>
	const T& ScriptField::Get(MonoObject* Object)
	{
		DE_CORE_ASSERT(false, "Cannot get undefined ScriptFieldType");

		return *((T*)FieldBuffer);
	}

	FIELD_TYPE_FUNCTIONS_DEFINITION(float, ScriptFieldType::Float);
	FIELD_TYPE_FUNCTIONS_DEFINITION(double, ScriptFieldType::Double);
	FIELD_TYPE_FUNCTIONS_DEFINITION(bool, ScriptFieldType::Bool);
	FIELD_TYPE_FUNCTIONS_DEFINITION(char, ScriptFieldType::Char);
	FIELD_TYPE_FUNCTIONS_DEFINITION(short, ScriptFieldType::Short);
	FIELD_TYPE_FUNCTIONS_DEFINITION(int, ScriptFieldType::Int);
	FIELD_TYPE_FUNCTIONS_DEFINITION(long, ScriptFieldType::Long);
	FIELD_TYPE_FUNCTIONS_DEFINITION(unsigned char, ScriptFieldType::Byte);
	FIELD_TYPE_FUNCTIONS_DEFINITION(unsigned short, ScriptFieldType::UShort);
	FIELD_TYPE_FUNCTIONS_DEFINITION(unsigned int, ScriptFieldType::UInt);
	FIELD_TYPE_FUNCTIONS_DEFINITION(unsigned long, ScriptFieldType::ULong);
	FIELD_TYPE_FUNCTIONS_DEFINITION(glm::vec2, ScriptFieldType::Vector2);
	FIELD_TYPE_FUNCTIONS_DEFINITION(glm::vec3, ScriptFieldType::Vector3);
	FIELD_TYPE_FUNCTIONS_DEFINITION(glm::vec4, ScriptFieldType::Vector4);
	// TODO: Implement Entity field set/get
}