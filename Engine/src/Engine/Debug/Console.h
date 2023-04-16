#pragma once
#pragma warning(disable : 4996)

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "Engine/Core/CallbackDispatcher.h"

#include <ctime>
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

	class Logger;

	class Console
	{
	public:
		Console() = default;
		Console(const WString& AppName)
			: AppName(AppName) { }

		virtual void OnAttach() = 0;

		virtual void OnDetach() = 0;

		template<typename... Args>
		void Log(const CString& LoggerName, LogLevel Level, const char* fmt, Args&&... args)
		{
			if (ConsoleLogger)
			{
				ConsoleLogger->Log(LoggerName, Level, fmt, std::forward<Args>(args)...);
			}
		}

	protected:
		WString AppName;

		Ref<Logger> ConsoleLogger = nullptr;
	};

	class Logger
	{
	public:
		Logger(CString Name)
			: Name(Name) { }

		struct LogData
		{
			LogLevel Level;

			CString Timestamp;
			CString LoggerName;

			CString LogString;
		};

		CallbackDispatcher<void, LogData>::CallbackHandle OnLog()
		{
			return OnLogDispatch.GetHandle();
		}

		template<typename... Args>
		void Log(const CString& LoggerName, LogLevel level, std::string_view fmt, Args&&... args)
		{
			std::time_t result = std::time(nullptr);

			char tbuff[128];
			std::strftime(tbuff, 128, "[%T]", std::localtime(&result));

			CString Timestamp = tbuff;

			CString LogString = TypeUtils::FormatUTF8(fmt.data(), std::forward<Args>(args)...);

			OnLogDispatch.Run({ level, Timestamp, LoggerName, LogString });
		}

	protected:
		CString Name;

		CallbackDispatcher<void, LogData> OnLogDispatch;
	};
}