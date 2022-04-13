#pragma once

#ifdef ENGINE_PLATFORM_WINDOWS
	#ifdef ENGINE_BUILD_DLL
		#define ENGINE_API __declspec(dllexport)
	#else
		#define ENGINE_API __declspec(dllimport)
	#endif
#else
	#error Dragon Engine only supports Windows
#endif

#ifdef ENGINE_BUILD_DEBUG
	#define ENGINE_ENABLE_ASSERTS
#endif

#ifdef ENGINE_ENABLE_ASSERTS
	#define DE_ASSERT(x, ...) { if(!x) { DE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define DE_CORE_ASSERT(x, ...) { if(!x) { DE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define DE_ASSERT(x, ...)
	#define DE_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)