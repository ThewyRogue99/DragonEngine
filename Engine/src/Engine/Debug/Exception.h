#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

namespace Engine
{
#define EXCEPTION_DEF(Name) \
	class Name : public Exception \
	{ \
	public: \
		Name(const std::string& msg) \
			: Exception(msg) { } \
	};

	class Exception
	{
	protected:
		Exception() = default;
		Exception(const std::string& msg)
			: Message(msg) { }

		const std::string& ExcMessage() const { return Message; }

		friend class ExceptionHandler;

	protected:
		std::string Message;
	};

	EXCEPTION_DEF(AssertException)
	EXCEPTION_DEF(ScriptException)
}