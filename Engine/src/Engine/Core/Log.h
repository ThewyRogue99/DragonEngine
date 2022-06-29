#pragma once
#include "Core.h"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace Engine
{
	class ENGINE_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }

		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;

		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core Log Macros

#define DE_CORE_TRACE(...) Engine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define DE_CORE_INFO(...) Engine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define DE_CORE_WARN(...) Engine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define DE_CORE_ERROR(...) Engine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define DE_CORE_CRITICAL(...) Engine::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client Log Macros

#define DE_TRACE(...) Engine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define DE_INFO(...) Engine::Log::GetClientLogger()->info(__VA_ARGS__)
#define DE_WARN(...) Engine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define DE_ERROR(...) Engine::Log::GetClientLogger()->error(__VA_ARGS__)
#define DE_CRITICAL(...) Engine::Log::GetClientLogger()->critical(__VA_ARGS__)
