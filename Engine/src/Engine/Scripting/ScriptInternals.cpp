#include "depch.h"
#include "ScriptInternals.h"

#include <mono/metadata/object.h>

namespace Engine
{
#define DE_ADD_INTERNAL_CALL(Name) mono_add_internal_call("DragonEngine.InternalCalls::" #Name, Name)

	static void Log_Native(MonoString* str)
	{
		const char* cstr = mono_string_to_utf8(str);

		DE_INFO(cstr);

		mono_free((void*)cstr);
	}

	void ScriptInternals::RegisterFunctions()
	{
		DE_ADD_INTERNAL_CALL(Log_Native);
	}
}