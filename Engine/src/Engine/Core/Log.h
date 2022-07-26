#pragma once
#include "Core.h"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#pragma warning(pop)

#include "Engine/Debug/Console.h"

namespace Engine
{
	class ENGINE_API Log
	{
	public:
		static void SetConsole(Ref<Console> NewConsole);

		inline static Ref<Console> GetConsole() { return DebugConsole; }

		inline static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }

		inline static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

		template<typename... Args>
		static void CoreTrace(spdlog::format_string_t<Args...> fmt, Args &&... args)
		{
			if (s_CoreLogger)
				s_CoreLogger->trace(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void CoreInfo(spdlog::format_string_t<Args...> fmt, Args &&... args)
		{
			if (s_CoreLogger)
				s_CoreLogger->info(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void CoreWarn(spdlog::format_string_t<Args...> fmt, Args &&... args)
		{
			if (s_CoreLogger)
				s_CoreLogger->warn(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void CoreError(spdlog::format_string_t<Args...> fmt, Args &&... args)
		{
			if (s_CoreLogger)
				s_CoreLogger->error(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void CoreCritical(spdlog::format_string_t<Args...> fmt, Args &&... args)
		{
			if (s_CoreLogger)
				s_CoreLogger->critical(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void ClientTrace(spdlog::format_string_t<Args...> fmt, Args &&... args)
		{
			if (s_ClientLogger)
				s_ClientLogger->trace(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void ClientInfo(spdlog::format_string_t<Args...> fmt, Args &&... args)
		{
			if (s_ClientLogger)
				s_ClientLogger->info(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void ClientWarn(spdlog::format_string_t<Args...> fmt, Args &&... args)
		{
			if (s_ClientLogger)
				s_ClientLogger->warn(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void ClientError(spdlog::format_string_t<Args...> fmt, Args &&... args)
		{
			if (s_ClientLogger)
				s_ClientLogger->error(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void ClientCritical(spdlog::format_string_t<Args...> fmt, Args &&... args)
		{
			if (s_ClientLogger)
				s_ClientLogger->critical(fmt, std::forward<Args>(args)...);
		}
	private:
		static Ref<Console> DebugConsole;

		static Ref<spdlog::logger> s_CoreLogger;

		static Ref<spdlog::logger> s_ClientLogger;
	};
}

// Core Log Macros

#define DE_CORE_TRACE(...) Engine::Log::CoreTrace(__VA_ARGS__)
#define DE_CORE_INFO(...) Engine::Log::CoreInfo(__VA_ARGS__)
#define DE_CORE_WARN(...) Engine::Log::CoreWarn(__VA_ARGS__)
#define DE_CORE_ERROR(...) Engine::Log::CoreError(__VA_ARGS__)
#define DE_CORE_CRITICAL(...) Engine::Log::CoreCritical(__VA_ARGS__)

// Client Log Macros

#define DE_TRACE(...) Engine::Log::ClientTrace(__VA_ARGS__)
#define DE_INFO(...) Engine::Log::ClientInfo(__VA_ARGS__)
#define DE_WARN(...) Engine::Log::ClientWarn(__VA_ARGS__)
#define DE_ERROR(...) Engine::Log::ClientError(__VA_ARGS__)
#define DE_CRITICAL(...) Engine::Log::ClientCritical(__VA_ARGS__)
