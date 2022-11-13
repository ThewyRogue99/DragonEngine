#pragma once
#include <memory>

#ifdef _WIN32
	#ifdef _WIN64
		#define ENGINE_PLATFORM_WINDOWS
	#else
		#error x86 Builds are not supported!
	#endif

#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>

	#if TARGET_IPHONE_SIMULATOR == 1
		#error IOS simulator is not supported!
	#elif TARGET_OS_IPHONE == 1
		#define ENGINE_PLATFORM_IOS
		#error IOS is not supported!
	#elif TARGET_OS_MAC == 1
		#define ENGINE_PLATFORM_MACOS
	#else
		#error Unknown Apple platform!
	#endif

#elif defined(__ANDROID__)
	#define ENGINE_PLATFORM_ANDROID
	#error Android is not supported!
#elif defined(__linux__)
	#define ENGINE_PLATFORM_LINUX
	#error Linux is not supported!
#else
	#error Unknown platform!
#endif

#ifdef ENGINE_DYNAMIC_LINK
	#ifdef __ENGINE__
		#define ENGINE_API __declspec(dllexport)
	#else
		#define ENGINE_API __declspec(dllimport)
	#endif
#else
	#define ENGINE_API
#endif

#if ENGINE_BUILD_DEBUG
	#define DE_ASSERT(x, ...) { if(!(x)) { DE_ERROR(__VA_ARGS__); __debugbreak(); } }
	#define DE_CORE_ASSERT(x, ...) { if(!(x)) { DE_CORE_ERROR(__VA_ARGS__); __debugbreak(); } }
#else
	#define DE_ASSERT(x, ...) { if(!(x)) { DE_ERROR(__VA_ARGS__); } }
	#define DE_CORE_ASSERT(x, ...) { if(!(x)) { DE_CORE_ERROR(__VA_ARGS__); } }
#endif

#define BIT(x) (1 << x)

#define BIND_CLASS_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
#define BIND_EVENT_FN(fn) BIND_CLASS_FN(fn)

#ifndef TEXT
#define TEXT(quote) L##quote
#endif

namespace Engine
{
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}