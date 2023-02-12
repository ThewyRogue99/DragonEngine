#pragma once
#include "Engine/Core/Core.h"

#include "Engine/Debug/Console.h"

namespace Engine
{
	class ENGINE_API Log
	{
	public:
		static void SetConsole(Ref<Console> NewConsole);

		inline static Ref<Console> GetConsole() { return DebugConsole; }

		template<typename... Args>
		static void ConsoleLog(const std::string& LoggerName, LogLevel Level, const char* fmt, Args&&... args)
		{
			if (DebugConsole)
			{
				DebugConsole->Log(LoggerName, Level, fmt, std::forward<Args>(args)...);
			}
		}

	private:
		static Ref<Console> DebugConsole;
	};
}

// Log Macros
#define DE_LOG(LoggerName, ...) Engine::Log::ConsoleLog(#LoggerName, LogLevel::Log, __VA_ARGS__)
#define DE_INFO(LoggerName, ...) Engine::Log::ConsoleLog(#LoggerName, LogLevel::Info, __VA_ARGS__)
#define DE_WARN(LoggerName, ...) Engine::Log::ConsoleLog(#LoggerName, LogLevel::Warning, __VA_ARGS__)
#define DE_ERROR(LoggerName, ...) Engine::Log::ConsoleLog(#LoggerName, LogLevel::Error, __VA_ARGS__)
#define DE_LOG_COMMAND(LoggerName, ...) Engine::Log::ConsoleLog(#LoggerName, LogLevel::Command, __VA_ARGS__)
