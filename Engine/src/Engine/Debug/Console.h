#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/format.h>
#pragma warning(pop)

#include <ctime>
#include <vector>
#include <string_view>

namespace Engine
{
	enum class LogLevel
	{
		Command = 0,
		Log,
		Warning,
		Error,
		Info,
	};

	class Console
	{
	public:
		Console() = default;

		virtual void OnAttach() = 0;

		virtual void OnDetach() = 0;

		friend class Log;

		struct ConsoleLogData
		{
			LogLevel Level;

			std::string Timestamp;
			std::string LoggerName;

			std::string LogString;
		};

		using LogListType = std::vector<ConsoleLogData>;

		const LogListType& GetLogs() { return LogList; }

		friend class CoreLogger;
		friend class ClientLogger;

	protected:
		CString AppName;

		virtual Ref<CoreLogger> GetCoreLogger() = 0;

		virtual Ref<ClientLogger> GetClientLogger() = 0;

		LogListType LogList = { };
	};

	class CoreLogger
	{
	public:
		CoreLogger(std::string Name, Console* AttachedConsole)
			: Name(Name), AttachedConsole(AttachedConsole) { }

		template<typename... Args>
		void Log(const char* fmt, Args &&... args)
		{
			Log_(Name, LogLevel::Log, fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void Info(const char* fmt, Args &&... args)
		{
			Log_(Name, LogLevel::Info, fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void Warn(const char* fmt, Args &&... args)
		{
			Log_(Name, LogLevel::Warning, fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void Error(const char* fmt, Args &&... args)
		{
			Log_(Name, LogLevel::Error, fmt, std::forward<Args>(args)...);
		}

		friend class Console;

	protected:
		template<typename... Args>
		void Log_(const std::string& LoggerName, LogLevel level, std::string_view fmt, Args&&... args)
		{
			std::time_t result = std::time(nullptr);

			char tbuff[128];
			std::strftime(tbuff, 128, "[%T]", std::localtime(&result));

			std::string Timestamp = tbuff;

			std::string LogString = fmt::vformat(fmt, fmt::make_format_args(args...));

			AttachedConsole->LogList.push_back({ level, Timestamp, LoggerName, LogString });
		}

	protected:
		std::string Name;

		Console* AttachedConsole;
	};

	class ClientLogger : public CoreLogger
	{
	public:
		ClientLogger(std::string Name, Console* AttachedConsole)
			: CoreLogger(Name, AttachedConsole) { }

		template<typename... Args>
		void ClientLog(const std::string& ClientName, const char* fmt, Args &&... args)
		{
			CoreLogger::Log_(ClientName, LogLevel::Log, fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void ClientInfo(const std::string& ClientName, const char* fmt, Args &&... args)
		{
			CoreLogger::Log_(ClientName, LogLevel::Info, fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void ClientWarn(const std::string& ClientName, const char* fmt, Args &&... args)
		{
			CoreLogger::Log_(ClientName, LogLevel::Warning, fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void ClientError(const std::string& ClientName, const char* fmt, Args &&... args)
		{
			CoreLogger::Log_(ClientName, LogLevel::Error, fmt, std::forward<Args>(args)...);
		}
	};
}