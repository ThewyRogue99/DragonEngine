#pragma once

#ifdef ENGINE_PLATFORM_WINDOWS
	#ifdef ENGINE_DYNAMIC_LINK
		#ifdef __ENGINE__
			#define ENGINE_API __declspec(dllexport)
		#elif defined(__SANDBOX__)
			#define ENGINE_API __declspec(dllimport)
		#else
			#define ENGINE_API
		#endif
	#else
		#define ENGINE_API
	#endif
#else
	#error Dragon Engine only supports Windows
#endif

#ifdef ENGINE_BUILD_DEBUG
	#define DE_ASSERT(x, ...) { if(!x) { DE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define DE_CORE_ASSERT(x, ...) { if(!x) { DE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define DE_ASSERT(x, ...)
	#define DE_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)