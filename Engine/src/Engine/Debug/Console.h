#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "Engine/Core/CallbackDispatcher.h"

#include <vector>
#include <deque>
#include <mutex>

namespace Engine
{
	class ConsoleStream
	{
	public:
		ConsoleStream(const ConsoleStream&) = delete;

		ENGINE_API ~ConsoleStream();

		friend class Console;

	public:
		enum class LogLevel
		{
			Command = 0,
			Log,
			Warning,
			Error,
			Info
		};

		typedef struct LogEntry
		{
			LogLevel Level = LogLevel::Log;

			CString Timestamp;
			CString LoggerName;

			CString LogString;

			uint64_t ThreadID = std::numeric_limits<uint64_t>::max();
			bool IsActive = false;
		} LogEntry;

	private:
		typedef struct LogBeginStruct
		{
			CString Timestamp;
			CString LoggerName;
		} LogBeginStruct;

	public:
		ENGINE_API static LogBeginStruct BeginLog(const CString& LoggerName);

		ENGINE_API static std::nullptr_t EndLog();

		ENGINE_API ConsoleStream& operator<<(const CString& str);

		ENGINE_API ConsoleStream& operator<<(std::nullptr_t ptr);

		ENGINE_API ConsoleStream& operator<<(const LogLevel& level);

		ENGINE_API ConsoleStream& operator<<(const LogBeginStruct& data);

		const std::vector<LogEntry*>& GetLogBuffer() const { return LogBuffer; }

	private:
		ENGINE_API ConsoleStream();

		ENGINE_API bool Thread_IsLogging(uint64_t ThreadID);

		ENGINE_API LogEntry* Thread_GetActiveLogEntry(uint64_t ThreadID);
		ENGINE_API bool Thread_SetActiveLogEntry(uint64_t ThreadID, LogEntry* NewEntry);

		ENGINE_API uint64_t Thread_GetID();

		ENGINE_API void EndCurrentEntry();

	private:
		LogEntry* CurrentEntry = nullptr;

		std::vector<LogEntry*> LogBuffer;

		std::deque<LogEntry*> LogEntryQueue;

		std::mutex LogMutex;
	};

	class Console
	{
	public:
		Console() = default;
		Console(const WString& AppName)
			: AppName(AppName) { }

		template<typename... Args>
		void Log(const CString& LoggerName, ConsoleStream::LogLevel Level, const char* fmt, Args&&... args)
		{
			CString LogString = TypeUtils::FormatUTF8(fmt, std::forward<Args>(args)...);

			_Log(LoggerName, Level, LogString);
		}

		ConsoleStream& GetStream() { return CStream; }

	private:
		ENGINE_API void _Log(const CString& LoggerName, ConsoleStream::LogLevel Level, const CString& LogString);

	private:
		WString AppName;

		ConsoleStream CStream;
	};
}