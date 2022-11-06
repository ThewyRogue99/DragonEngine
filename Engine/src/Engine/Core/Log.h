#pragma once
#include "Core.h"

#include "Engine/Debug/Console.h"

namespace Engine
{
	class ENGINE_API Log
	{
	public:
		static void SetConsole(Ref<Console> NewConsole);

		inline static Ref<Console> GetConsole() { return DebugConsole; }

		inline static Ref<CoreLogger>& GetCoreLogger() { return s_CoreLogger; }

		inline static Ref<ClientLogger>& GetClientLogger() { return s_ClientLogger; }

		template<typename... Args>
		static void CoreLog(const char* fmt, Args &&... args)
		{
			if (s_CoreLogger)
				s_CoreLogger->Log(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void CoreInfo(const char* fmt, Args &&... args)
		{
			if (s_CoreLogger)
				s_CoreLogger->Info(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void CoreWarn(const char* fmt, Args &&... args)
		{
			if (s_CoreLogger)
				s_CoreLogger->Warn(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void CoreError(const char* fmt, Args &&... args)
		{
			if (s_CoreLogger)
				s_CoreLogger->Error(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void ClientLog(const char* fmt, Args &&... args)
		{
			if (s_ClientLogger)
				s_ClientLogger->Log(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void ClientInfo(const char* fmt, Args &&... args)
		{
			if (s_ClientLogger)
				s_ClientLogger->Info(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void ClientWarn(const char* fmt, Args &&... args)
		{
			if (s_ClientLogger)
				s_ClientLogger->Warn(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void ClientError(const char* fmt, Args &&... args)
		{
			if (s_ClientLogger)
				s_ClientLogger->Error(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void ClientLog(const std::string& ClientName, const char* fmt, Args &&... args)
		{
			if (s_ClientLogger)
				s_ClientLogger->ClientLog(ClientName, fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void ClientInfo(const std::string& ClientName, const char* fmt, Args &&... args)
		{
			if (s_ClientLogger)
				s_ClientLogger->ClientInfo(ClientName, fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void ClientWarn(const std::string& ClientName, const char* fmt, Args &&... args)
		{
			if (s_ClientLogger)
				s_ClientLogger->ClientWarn(ClientName, fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void ClientError(const std::string& ClientName, const char* fmt, Args &&... args)
		{
			if (s_ClientLogger)
				s_ClientLogger->ClientError(ClientName, fmt, std::forward<Args>(args)...);
		}

	private:
		static Ref<Console> DebugConsole;

		static Ref<CoreLogger> s_CoreLogger;

		static Ref<ClientLogger> s_ClientLogger;
	};
}

// Core Log Macros

#define DE_CORE_LOG(...) Engine::Log::CoreLog(__VA_ARGS__)
#define DE_CORE_INFO(...) Engine::Log::CoreInfo(__VA_ARGS__)
#define DE_CORE_WARN(...) Engine::Log::CoreWarn(__VA_ARGS__)
#define DE_CORE_ERROR(...) Engine::Log::CoreError(__VA_ARGS__)

// Client Log Macros

#define DE_LOG(...) Engine::Log::ClientLog(__VA_ARGS__)
#define DE_INFO(...) Engine::Log::ClientInfo(__VA_ARGS__)
#define DE_WARN(...) Engine::Log::ClientWarn(__VA_ARGS__)
#define DE_ERROR(...) Engine::Log::ClientError(__VA_ARGS__)
