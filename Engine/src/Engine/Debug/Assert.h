#pragma once

#include "Engine/Types/Types.h"
#include "ExceptionHandler.h"

#if ENGINE_BUILD_DEBUG
	#define DE_ASSERT(x, ...) { if(!(x)) { __debugbreak(); ExceptionHandler::ThrowException<AssertException>(TypeUtils::FormatUTF8(__VA_ARGS__)); } }
#else
	#define DE_ASSERT(x, ...) { if(!(x)) { ExceptionHandler::ThrowException<AssertException>(TypeUtils::FormatUTF8(__VA_ARGS__)); } }
#endif