#pragma once
#include "Engine/Core/Core.h"

#include "Engine/Debug/Console.h"

#include <vector>

namespace Engine
{
	using LogLevel = ConsoleStream::LogLevel;

	class Log
	{
	public:
		ENGINE_API static void SetConsole(Ref<Console> NewConsole);

		inline static Ref<Console> GetConsole() { return DebugConsole; }

		template<typename... Args>
		static void ConsoleLog(const CString& LoggerName, LogLevel Level, const char* fmt, Args&&... args)
		{
			if (DebugConsole)
			{
				DebugConsole->Log(LoggerName, Level, fmt, std::forward<Args>(args)...);
			}
		}

	private:
		ENGINE_API static Ref<Console> DebugConsole;
	};
}

// Log Macros
#define DE_LOG(LoggerName, ...) Engine::Log::ConsoleLog(#LoggerName, Engine::LogLevel::Log, __VA_ARGS__)
#define DE_INFO(LoggerName, ...) Engine::Log::ConsoleLog(#LoggerName, Engine::LogLevel::Info, __VA_ARGS__)
#define DE_WARN(LoggerName, ...) Engine::Log::ConsoleLog(#LoggerName, Engine::LogLevel::Warning, __VA_ARGS__)
#define DE_ERROR(LoggerName, ...) Engine::Log::ConsoleLog(#LoggerName, Engine::LogLevel::Error, __VA_ARGS__)
#define DE_LOG_COMMAND(LoggerName, ...) Engine::Log::ConsoleLog(#LoggerName, Engine::LogLevel::Command, __VA_ARGS__)
