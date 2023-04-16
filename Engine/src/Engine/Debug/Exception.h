#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

namespace Engine
{
#define EXCEPTION_DEF(Name) \
	class Name : public Exception \
	{ \
	public: \
		Name(const CString& msg) \
			: Exception(msg) { } \
	};

	class Exception
	{
	protected:
		Exception() = default;
		Exception(const CString& msg)
			: Message(msg) { }

		const CString& ExcMessage() const { return Message; }

		friend class ExceptionHandler;

	protected:
		CString Message;
	};

	EXCEPTION_DEF(AssertException)
	EXCEPTION_DEF(ScriptException)
}